#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

#define BUFFER_SIZE 20
#define NUM_ITEMS 30

int buffer[BUFFER_SIZE];
int sum = 0;
int in = 0, out = 0, count =0;
pthread_mutex_t mutex_buffer;
pthread_mutex_t mutex_sum;
pthread_cond_t buffer_not_empty_cond;
pthread_cond_t buffer_not_full_cond;
void *producer(void *arg){
  for(int i = 1; i <= NUM_ITEMS; i++){
    pthread_mutex_lock(&mutex_buffer);
    while(count == BUFFER_SIZE){
      pthread_cond_wait(&buffer_not_full_cond, &mutex_buffer);
    }
    buffer[in] = i;
    in = (in+1)%BUFFER_SIZE;
    count++;
    pthread_cond_signal(&buffer_not_empty_cond);
    pthread_mutex_unlock(&mutex_buffer);
  }
  return NULL;
}
void *consumer(void *arg){
  int item;
  for(int i = 0; i < NUM_ITEMS; i++){
    pthread_mutex_lock(&mutex_buffer);
    while(count == 0){
      pthread_cond_wait(&buffer_not_empty_cond, &mutex_buffer);
    }
    item = buffer[out];
    out = (out+1)%BUFFER_SIZE;
    count--;
    pthread_cond_signal(&buffer_not_full_cond);
    pthread_mutex_unlock(&mutex_buffer);
    pthread_mutex_lock(&mutex_sum);
    sum = sum + item;
    pthread_mutex_unlock(&mutex_sum);
  }
  return NULL;
}
int main()
{
  pthread_t thread1,thread2,thread3,thread4;
  pthread_mutex_init(&mutex_buffer,NULL);
  pthread_mutex_init(&mutex_sum, NULL);

  pthread_create(&thread1, NULL, producer, NULL);
  pthread_create(&thread2, NULL, producer, NULL);
  pthread_create(&thread3, NULL, consumer, NULL);
  pthread_create(&thread4, NULL, consumer, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);
  pthread_join(thread4, NULL);

  printf("SUM is: %d\n", sum);
  return 0;
}
