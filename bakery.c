#include "pthread.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#define MEMBAR __sync_synchronize()
#define THREAD_COUNT 8
volatile int tickets[THREAD_COUNT];
volatile int choosing[THREAD_COUNT];

int shared_variable=0;
volatile int resource;
void lock(int thread){
    choosing[thread] = 1; //Before getting the ticket number choose variable is set to be true
    int max_ticket = 0;
    for (int i = 0; i < THREAD_COUNT; ++i) { // Finding Maximum ticket value among current threads
        int ticket = tickets[i];
        max_ticket= ticket > max_ticket ? ticket : max_ticket;
    }
    tickets[thread] = max_ticket + 1;// Allotting a new ticket value as MAXIMUM + 1
    choosing[thread] = 0;
    for (int other = 0; other < THREAD_COUNT; ++other) {// Applying the bakery algorithm conditions
        while (choosing[other]) {
        }
        while(tickets[other]!=0&&(tickets[other]<tickets[thread]||
            (tickets[other]==tickets[thread]&&other<thread))){
        }
    }
}

// EXIT Section
void unlock(int thread){
    //MEMBAR;
    tickets[thread] = 0;
}

// The CRITICAL Section
void use_resource(int thread){
    int thread_id = thread;
    int i;
    for (i = 0; i < 100000; i++) {
        int temp = shared_variable;
        temp++;
        shared_variable = temp;
    }
    printf("Thread %d finished. Shared variable: %d\n", thread_id, shared_variable);
}

// A simplified function to show the implementation
void* thread_body(void* arg){
    long thread = (long)arg;
    lock(thread);
    use_resource(thread);
    unlock(thread);
    return NULL;
}


int main(int argc, char** argv){
    memset((void*)tickets, 0, sizeof(tickets));
    memset((void*)choosing, 0, sizeof(choosing));
    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < 8; ++i) {//"thread_body" as its thread routine
        pthread_create(&threads[i], NULL, &thread_body,(void*)((long)i));
    }
    
    for (int i = 0; i < 8; ++i) {
        // Reaping the resources used by all threads once their task is completed !
        pthread_join(threads[i], NULL);
    }
    printf("shared_variable: %d:",shared_variable);
    return 0;
}
