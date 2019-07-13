#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define NUM_READERS 5
#define NUM_WRITERS 2

#define NUM_READS 3
#define NUM_WRITES 3

int subject = -1;
int num_readers = 0;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t r = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_reader = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_writer = PTHREAD_COND_INITIALIZER;

void * my_read(void * param);
void * my_write(void * param);

int main(int argc, char * argv[]) {
  srand(time(0));
  pthread_t r1, r2, r3, r4, r5, w1, w2, w3, w4, w5;
  pthread_t readers[] = {r1, r2, r3, r4, r5};
  pthread_t writers[] = {w1, w2, w3, w4, w5};

  for (int x = 0; x < NUM_READERS; x++) {
    if (pthread_create(&readers[x], NULL, my_read, NULL) != 0) {
      fprintf(stderr, "Unable to create read thread!\n");
      exit(1);
    }
  }

  for (int i = 0; i < NUM_WRITERS; i++) {
    if (pthread_create(&writers[i], NULL, my_write, &i) != 0) {
      fprintf(stderr, "Unable to create write thread!\n");
      exit(1);
    }
    sleep(1);
  }
  

  for (int a = 0; a < NUM_READERS; a++) {
    pthread_join(readers[a], NULL);    
  }

  for (int r = 0; r < NUM_WRITERS; r++) {
    pthread_join(writers[r], NULL);    
  }
  
  printf("Parent complete.\n");
}

void * my_read(void * param) {
  sleep(rand() % 4);  
  pthread_mutex_lock(&m);
  
  /* pthread_mutex_lock(&r); */
  /* printf("Currently %d readers\n", num_readers); fflush(stdout);       */
  num_readers++;
  
  for (int y = 0; y < NUM_READS; y++) {

    printf("Read value %d with %d readers present\n", subject, num_readers); fflush(stdout);
  }

  num_readers--;
  /* pthread_mutex_unlock(&r); */

  pthread_mutex_unlock(&m);
  pthread_cond_broadcast(&c_writer);
  
}

void * my_write(void * param) {
  sleep(rand() % 2);  
  int * i = (int *) param;
  /* printf("i = %d\n", *i); fflush(stdout);   */
  pthread_mutex_lock(&m);  
  for (int j = 0; j < NUM_WRITES; j++) {


    while (num_readers > 0) {
      printf("Waiting on %d readers\n", num_readers); fflush(stdout);
      pthread_cond_wait(&c_writer, &m);
    }
    
    subject = (*i*10) + j;
    printf("Write value %d with %d readers present\n", subject, num_readers); fflush(stdout);
    /* pthread_cond_signal(&c_writer); */

  }
  pthread_mutex_unlock(&m);    
  /* pthread_cond_signal(&c_reader);   */
}
