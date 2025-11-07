#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h> // compile by linking "-lpthread -lrt"

#define QUEUE_SIZE 5

typedef struct {
    char s[30]; //struct for information about the order itself
    //int type; maybe not in case of two order queues
    int customerID;
} Order;

typedef struct {
    Order items[QUEUE_SIZE]; //change to be variable
    int front;      //Variables to help with indexing for FIFO 
    //int rear; //index not size?
    int count;
} Queue;

void _init_(Queue *q){
    q->front = -1;
    //q->size = size;
    //q->rear = -1;
    q->count = 0; 
}

void enqueue(Queue *qu, Order meal){ 
    
    if(qu->count < QUEUE_SIZE){
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

    /*Immediately enqueues something that's like a record. don't make this array too small.
    waiting_count++
    wait but in a way so waiter can pull you out, and when you done waiter also knows
    wating_count--*/

    //pthread_t CID = pthread_self();
    
    //waiting for a wake up from waiter

    /*Make sure the queue is not full in a while loop
    wait for mutex to be free and atomically do ...
    get pthread_mutex (the called cs will have a predicate for allowing people in)
    create an order. ID from (*int)CID. Message aspect will come from the lab3 array randomizer.
    enqueue an order. 
    release lock on the order-to-chef queue.*/
    
    //Customer is waiting for a signal from the waiter with its specific condition variable

    //sem_post(&table_sem);          
    
    //Decrements global variable of total customers
    return NULL;
}

void* waiter(void* arg){ //These functions must return void* to be used with pthread
    
    //Loop, while remaining_customers != 0
    
    /*sem_wait(&table_sem);
    Code to take a wating customer off of wait queue*/

    //wake up a random customer. (they order)

     /*Make sure the queue is not empty in a while loop
    wait for mutex to be free and atomically do ...
    get pthread_mutex
    customerOrder = dequeue(completed_order_queue);
    release lock on the completed_order queue.*/
     
    //wake up customer with the same cv as its CID
    
    return NULL;
}

void* chef(void* arg){
    
    //Loop, while remaining_customers != 0
    
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
    
    //_init_(&pending_order_queue); and completed queue
    
    //three arrays for sizes of functions (for IDs?)
    
    if(/*pthread_create(pthread_t &wthread, NULL, waiter, NULL)*/ 0 != 0){ 
        perror("Thread creation failed");
        exit(1);
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