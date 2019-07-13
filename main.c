#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define NUM_READERS 5
#define NUM_WRITERS 5

#define NUM_READS 3
#define NUM_WRITES 3

int subject = -1;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_reader = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_writer = PTHREAD_COND_INITIALIZER;

void * my_read(void * param);
void * my_write(void * param);

int main(int argc, char * argv[]) {
  srand(time(0));
  pthread_t tid1, tid2;

  if (pthread_create(&tid1, NULL, my_read, NULL) != 0) {
    fprintf(stderr, "Unable to create read thread!\n");
    exit(1);
  }

  if (pthread_create(&tid2, NULL, my_write, NULL) != 0) {
    fprintf(stderr, "Unable to create write thread!\n");
    exit(1);
  }

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  printf("Parent complete.\n");
}

void * my_read(void * param) {
  for (int x = 0; x < NUM_READERS; x++) {
    for (int y = 0; y < NUM_READS; y++) {
      sleep(rand() % 2);
      pthread_mutex_lock(&m);
      printf("Read value %d\n", subject); fflush(stdout);
      pthread_mutex_unlock(&m);
      pthread_cond_signal(&c_writer);
    }
  }
}

void * my_write(void * param) {
  for (int i = 0; i < NUM_WRITERS; i++) {
    for (int j = 0; j < NUM_WRITES; j++) {
      sleep(rand() % 2);      
      pthread_mutex_lock(&m);
      subject = (i*10) + j;
      printf("Write value %d\n", subject); fflush(stdout);
      pthread_mutex_unlock(&m);
      pthread_cond_signal(&c_reader);
    }
  }
}
