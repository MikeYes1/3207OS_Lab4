#include <stdio.h>
#include <pthread.h>

#define QUEUE_SIZE 5

typedef struct {
    char s[30]; //struct for information about the order itself
    int type;
    int customerID;
} Order;

typedef struct {
    Order items[QUEUE_SIZE];  
    int front;      //Variables to help with indexing for FIFO 
    int rear;
    int count;
} Queue;

void* customer(void* arg){    //generate data and put it in the buffer

    //get pthread_mutex
    //make empty or non empty           Needs to check before writing to buffer each time, atomic too. Avoid overflow.
                                       
    return NULL;
}

void* waiter(void* arg){ //These functions must return void* to be used with pthread
    
    
    /*pthread_exit(NULL) to terminate, not necessary just to reach return but main
    can call it to let other threads run*/
    return NULL;
}

void* chef(void* arg){
    
    
    return NULL;
}


int main(int argc, char **argv)
{
    
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



race conditions
two threads try to change a condition variable based on the current value they have



deadlock
when threads are blocked forever since they're waiting for eachother to release an occupied source to continue

nd starvation maybe

 

*/