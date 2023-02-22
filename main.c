#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 20

int buffer[BUFFER_SIZE]; //shared buffer
int sum = 0; //shared variable
int in = 0; // index to insert or write item into buffer 
int out = 0; // index to read item from the buffer. 
int count = 0; //variable to count number of items in the buffer

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex for buffer access 
pthread_cond_t full = PTHREAD_COND_INITIALIZER; //conditional variable to check buffer is not empty
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; //conditional variable to check buffer is empty

//producer method to produce items
void *producer(void *arg) {
    int i, value;
    for (i = 1; i <= 40; i++) {
        value = i;
        pthread_mutex_lock(&mutex); //acquire lock on the buffer
      
        //wait until the buffer is empty
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&empty, &mutex);
        }
        //inserting value into buffer
        buffer[in] = value;
        in = (in + 1) % BUFFER_SIZE;
        count++;
      
        //signal that buffer is empty
        pthread_cond_signal(&full);
        
        //release lock on buffer
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg) {
    int value;
    for(int i = 0; i < 40;i++){
        pthread_mutex_lock(&mutex); //acquire lock on buffer
      
        //wait until buffer is not empty
        while (count == 0) {
            pthread_cond_wait(&full, &mutex);
        }
      
        //read value from buffer
        value = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        sum += value; //update sum
        
        //signal that buffer is empty
        pthread_cond_signal(&empty);
        
        //release lock on buffer
        pthread_mutex_unlock(&mutex);
        /*if (count == 0) {
            break;
        }*/
    }
    return NULL;
}

int main() {
    pthread_t p1, p2, c1, c2; //declaring threads using pthread
    //creating producer threads
    pthread_create(&p1, NULL, producer, NULL);
    pthread_create(&p2, NULL, producer, NULL);
    //creating consumer threads
    pthread_create(&c1, NULL, consumer, NULL);
    pthread_create(&c2, NULL, consumer, NULL);
    //wait for producer threads to finish
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    //wait for consumer threads to finish
    pthread_join(c1, NULL);
    pthread_join(c2, NULL);
    
    //finally priniting sum
    printf("SUM = %d\n", sum);
    return 0;
}
