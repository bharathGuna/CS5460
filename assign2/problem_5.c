#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

volatile int tickets[99];
volatile int choosing[99];
volatile int in_cs = 0;
volatile int done = 0;
int THREADCOUNT = 0;

typedef struct _thread_data_t {
  int tid;
} thread_data_t;


typedef struct spin_lock_t{
  volatile int wait;
  volatile int serve;
} spin_lock_t;

spin_lock_t lock;

/*
 * atomic_xadd
 *
 * equivalent to atomic execution of this code:
 *
 * return (*ptr)++;
 * 
 */
static inline int atomic_xadd(volatile int *ptr) {
  register int val __asm__("eax") = 1;
  asm volatile("lock xaddl %0,%1"
  : "+r" (val)
  : "m" (*ptr)
  : "memory"
  );  
  return val;
}


void spin_lock (struct spin_lock_t *s)
{
  int turn = atomic_xadd(&s->wait);
  //printf("%d\n",turn);
  while(turn != s->serve);
}
void spin_unlock (struct spin_lock_t *s)
{
 atomic_xadd(&s->serve); 
}

void* thread_body(void *arg)
{
  
  thread_data_t *data = (thread_data_t *)arg;
  int val = data->tid;
  
  int count = 0;
  while(1)
    {
      spin_lock(&lock);
      assert(in_cs==0);
      in_cs++;
 
      assert(in_cs==1);
      in_cs++;
 
      assert(in_cs==2);
      in_cs++;
  
      assert(in_cs==3);
      in_cs=0;
 
      spin_unlock(&lock);
   
      count++;
      if(done)
	{
	  printf("Thread %d in cs %d\n",val,count);
	  pthread_exit(NULL);
	}
    }
}

int main(int argc, char **argv)
{

  if(argc != 3){
    printf("Usage: ./a.out <Num Threads> <Time in Sec>");
    return 0;
  }

  THREADCOUNT = atoi(argv[1]);
  int sec = atoi(argv[2]);
 
  pthread_t threads[THREADCOUNT];
  int i;
  thread_data_t thr_data[THREADCOUNT];
  lock.wait = 0;
  lock.serve = 0;
  for(i = 0; i < THREADCOUNT; ++i)
    {
   
      thr_data[i].tid = i;
      if(pthread_create(&threads[i],NULL, &thread_body, &thr_data[i]))
	{
	  printf("Failed to create thread");
	  return 1;
	}
    }
  
  sleep(sec);
  done = 1;
  for(i = 0; i < THREADCOUNT; ++i)
    {
      pthread_join(threads[i],NULL);
    }
  return 0;
}
