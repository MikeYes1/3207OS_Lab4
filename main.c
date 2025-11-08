#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h> // compile by linking "-lpthread -lrt"
#include <stdlib.h>

#define BUFF_SIZE 5

sem_t table_sema, customer_sema;
pthread_mutex_t waiting_mutex, order_mutex, complete_order_mutex;
pthread_cond_t seat_customer = PTHREAD_COND_INITIALIZER;
int waiting_count = 0;
int total_customers;
//int customer_number;

typedef struct {
    char s[30]; //struct for information about the order itself
    //int type; maybe not in case of two order queues
    pthread_cond_t customerID;
} Order;

typedef struct { //Help from ChatGPT to make a variable sized array
    int size;
    int front;      //Variable to help with indexing for FIFO 
    //int rear;
    int count;
    pthread_cond_t not_full, not_empty;
    //sem_t sem; //Not really part of the queue, but pthread_create can only pass one argument, so this struct
              //should include everything that's needed for the functions.
    Order items[]; //change to be variable
} Queue;

void _init_(Queue *q, int size/*,sem_t table_sema*/){
    q->size = size; 
    q->front = -1;
    //q->rear = -1;
    q->count = 0; 
    //q->sem = table_sema;
    q->not_full = PTHREAD_COND_INITIALIZER;
    q->not_empty = PTHREAD_COND_INITIALIZER;    
}

void enqueue(Queue *qu, Order meal){ 
    
    if(qu->count < qu->size){
        for(int i = qu->front; i > -1; i--){ //Doesn't run this loop when the queue's empty
            qu->items[i + 1] = qu->items[i];
        }
        qu->items[0] = meal; //Always copy the new item to the back
        qu->front++;
        qu->count++;
    } else {
        printf("\nBuffer is full\n"); //no need to block, that's done elsewhere
    }
    
    /*put in first slot if empty.
    and still put in first slot whenever, just copy the element to the next slot over each time until no more
    don't lose elements, when count is full.
    */
    
}//put in back, take from front.

Order dequeue(Queue *qu){
    
    Order temp;
    temp.customerID = -1; //An Order variable which returns silly values if the buffer is empty
    strcpy(temp.s, "ew");
    
    if(qu->count != 0){
        temp = qu->items[qu->front]; 
        //Originally I wanted to remove the element at the front, but I think it's easier to ignore it entirely
        qu->front--;
        qu->count--;
        return temp; 
    } else {
        printf("\nBuffer is empty\n");
        return temp;
    }
}

void* customer(void* arg){    //generate data and put it in the buffer
    
    Queue **queues = arg; //Pointer array of two pointers to struct... so confusing. I used ChatGPT to help.
    
    Order arrival;
    pthread_cond_init(&arrival.customerID, NULL); //or random junk value. hopefully this saves value then decrements...
    strcpy(arrival.s, "waiting");
    
    //mutex lock?
    //cond_wait NEEDS the pthread_mutex_t mutex

    pthread_mutex_lock(&waiting_mutex); //acquires
    while(queues[0]->count == queues[0]->size){ //while the queue is full...
        pthread_cond_wait(&queues[0]->not_full, &waiting_mutex); //releases and blocks. signalled by a waiter obtaining table
    }
    enqueue(queues[0], arrival);
    waiting_count++;
    pthread_cond_signal(&queues[0]->not_empty);
    pthread_cond_wait(&seat_customer, &waiting_mutex); //block for waiter
    waiting_count--; 
    pthread_mutex_unlock(&waiting_mutex); 

    
    strcpy(arrival.s, "order up!");
    
    pthread_mutex_lock(&order_mutex);

    while(queues[1]->count == queues[1]->size){ //while the queue is full...
        pthread_cond_wait(&queues[1]->not_full, &order_mutex); //releases and blocks. signalled by a waiter obtaining table
    }
    enqueue(queues[1], arrival);//will have an array of Queue object
    pthread_cond_signal(&queues[1]->not_empty);
    
    pthread_cond_wait(&arrival.customerID, &order_mutex);//<---- dangerous??
    pthread_mutex_unlock(&order_mutex); 

    sem_post(&table_sema); 
    
    printf("\nCustomer %d done\n", total_customers);
    total_customers--;
    return NULL;
}

void* waiter(void* arg){ //These functions must return void* to be used with pthread
    
    Queue **queues = arg;
    Order order;
    
    //Loop, while remaining_customers != 0
    while(total_customers != 0){
        
        sem_wait(&table_sema); //in lock? 
        
        pthread_mutex_lock(&waiting_mutex);
        while(queues[0]->count != 0){
            pthread_cond_wait(&queues[0]->not_empty, &waiting_mutex);
        }
        dequeue(queues[0]);
        pthread_cond_signal(&queues[0]->not_full);
        
        //if waiting_count != 0?
        pthread_cond_signal(&seat_customer);
        pthread_mutex_unlock(&waiting_mutex);
        
        //mutex for completed order
        
        pthread_mutex_lock(&complete_order_mutex);
        while(queues[1]->count != 0){
            pthread_cond_wait(&queues[1]->not_empty, &waiting_mutex);
        }
        order = dequeue(queues[1]);
        pthread_cond_signal(&queues[1]->not_full);        
        pthread_mutex_unlock(&complete_order_mutex);     

        pthread_cond_signal(&order.customerID); //in lock? probably doesn't matter
   
    }
    
    
    /*sem_wait(&table_sem);
    Code to take a wating customer off of wait queue*/

    //wake up a random customer. (they order
    
    return NULL;
}

void* chef(void* arg){
    
    //Loop, while remaining_customers != 0
    Queue **queues = arg;
    Order order;
    
    while(total_customers != 0){
        pthread_mutex_lock(&order_mutex);
        while(queues[0]->count != 0){
            pthread_cond_wait(&queues[0]->not_empty, &order_mutex);
        }
        order = dequeue(queues[0]);
        pthread_cond_signal(&queues[0]->not_full);
        pthread_mutex_unlock(&order_mutex);
    
    
        pthread_mutex_lock(&complete_order_mutex); //acquires
        while(queues[1]->count == queues[1]->size){ //while the queue is full...
            pthread_cond_wait(&queues[1]->not_full, &complete_order_mutex); //releases and blocks. signalled by a waiter obtaining table
        }
        enqueue(queues[1], order);
        pthread_cond_signal(&queues[1]->not_empty);
        pthread_mutex_unlock(&waiting_mutex); 
    }
    
    /*lock acquire of order queue
    order = dequeue
    release
     */ 
    
    /*lock acquire of completed order queue
    enqueue
    release
     */ 
    
    return NULL;
}


int main(int argc, char **argv)
{
    int num_tables, num_waiters, num_chefs, waiting_capacity;
    
    
    printf("Welcome yo\n\nEnter the amount of total customers: ");
    scanf("%d", &total_customers);
    printf("Enter the amount of tables: ");
    scanf("%d", &num_tables);
    printf("Enter the amount of waiters: ");
    scanf("%d", &num_waiters);   
    printf("Enter the amount of chefs: ");
    scanf("%d", &num_chefs);    
    printf("Enter the amount of customers that can wait at any given time (please be a high number): ");
    scanf("%d", &waiting_capacity);
    
    int num_customers = total_customers;
    
    sem_init(&table_sema, 0, num_tables); //not static since global?
    sem_init(&customer_sema, 0, 1);
    
    pthread_mutex_init(&waiting_mutex, NULL);
    pthread_mutex_init(&order_mutex, NULL);
    pthread_mutex_init(&complete_order_mutex, NULL);
    
    pthread_t customer_threads[num_customers];
    pthread_t waiter_threads[num_waiters];
    pthread_t chef_threads[num_chefs];
    
    //Create all the queues, which are pointers
    //Didn't feel like making this a seperate function even if it's repetative here
    Queue *waiting_queue = malloc(sizeof(Queue) + sizeof(Order) * waiting_capacity);
    _init_(waiting_queue, waiting_capacity);
    Queue *order_queue = malloc(sizeof(Queue) + sizeof(Order) * BUFF_SIZE);
    _init_(order_queue, BUFF_SIZE);    
    Queue *complete_order_queue = malloc(sizeof(Queue) + sizeof(Order) * BUFF_SIZE);
    _init_(complete_order_queue, BUFF_SIZE);
    
    Queue **argument_for_customer[2] = {&waiting_queue, &order_queue};
    Queue **argument_for_waiter[2] = {&waiting_queue, &complete_order_queue};
    Queue **argument_for_chef[2] = {&order_queue, &complete_order_queue};
    

    for(int i = 0; i < num_customers; i++){
        if(pthread_create(&customer_threads[i], NULL, customer, argument_for_customer) != 0){ 
            perror("Thread creation failed");
            exit(1);
        }
    }
    
    for(int i = 0; i < num_waiters; i++){
        if(pthread_create(&waiter_threads[i], NULL, waiter, argument_for_waiter) != 0){ 
            perror("Thread creation failed");
            exit(1);
        }
    }
    
    for(int i = 0; i < num_chefs; i++){
        if(pthread_create(&chef_threads[i], NULL, chef, argument_for_chef) != 0){ 
            perror("Thread creation failed");
            exit(1);
        }
    }
    
    
    
    
    
    //just make the threads here and that's it. do the function to stop main or to let chefs and waiters there's no more customers
    
    /*
     * hmmm..
     * im working with loops and pthread_create for this mutex solution.
     * 
     * System to change access to the CS to the concurrent threads? based on blocking?
     * 
     * waiter loops and chef. signal maybe
     * customer lock and cv
     * main not too complicated 
     * call customer whenever we want, doesn't go into queue itself. will waiter mutex happen in main, customer, or itself?
     * 
     */
    
     
    //synchronize with pthread_join(something_thread, NULL)
    
    
    printf("Hello World\n");
    
    pthread_mutex_destroy(&waiting_mutex);
    pthread_mutex_destroy(&order_mutex);
    pthread_mutex_destroy(&complete_order_mutex);

    
    free(waiting_queue);
    free(order_queue);
    free(complete_order_queue);
    
    pthread_exit(NULL);
    
    return 0;
}
/*

threads have they own stack

pthread_cancel to send a request to a running thread to terminate once it enters a certain state
pthread_self to get your thread ID
pthread_exit(NULL) to terminate, not necessary if you want return to be the end of everything
    but main can call it to let other threads run
    

p&c buffer
pro can't add data when the buffer is full. sleep. 
cons can't take data when the buffer is empty. sleep.
both can't access it at the same time
they have to wake each other up. Consumer notifies the producer when it takes from the buffer and vice versa. be careful of deadlock


Stack Overflow answer:
1. lock the mutex
2. test the predicate
3. if the predicate is true then go to (6)
4. wait on the CV
5. upon returning from the wait, go to (2)
6. [optional] do stuff under protection of the mutex
7. unlock the mutex

semaphore
controls access to "tables", taking from the wait queue.
(customers should block on a full wait queue).
sem_wait to decrement and sem_post to signal
sem_t urmom
sem_init(&urmom, 0, NUM_TABLES) if == -1 then perror
sem_wait if 0 then wait
acts like a guard

race conditions
two threads try to change a condition variable based on the current value they have



deadlock
when threads are blocked forever since they're waiting for eachother to release an occupied source to continue

nd starvation maybe

 

to stop waiters and chefs, have each customer whittle the amount of customers down to 0 at the end.
chefs and waiters will loop until that value is zero.




table conuting semaphore
locks, mutual exclusion, and condition variables
main function; loop pthread_create()
copy and paste some easy stuff from project 3


*/