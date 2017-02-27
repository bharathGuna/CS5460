#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

volatile int tickets[99];
volatile int choosing[99];
volatile int in_cs = 0;
volatile int done = 0;
int THREADCOUNT = 0;

typedef struct _thread_data_t {
  int tid;
} thread_data_t;

void lock(int id)
{

  choosing[id] = 1;
  //Finding the max ticket
  int max_ticket = 0;
  int j,t;
  for( j = 0; j < THREADCOUNT; ++j)
    {
      int ticket = tickets[j];
      max_ticket = ticket > max_ticket ? ticket:max_ticket;
    }
  
  tickets[id]= max_ticket+1;
  choosing[id] = 0;
  
  for( t = 0; t < THREADCOUNT; ++t)
    {
      while(choosing[t]){}
      while(tickets[t]!= 0 &&
	    (tickets[id] > tickets[t] ||( tickets[id] == tickets[t] && id > t))){
	sched_yield();
      }
      
    }
  
}
void unlock(int i)
{
  tickets[i] = 0;
}

void* thread_body(void *arg)
{
  
  thread_data_t *data = (thread_data_t *)arg;
  int val = data->tid;

  int count = 0;
  while(1)
    {
      lock(val);
      assert(in_cs==0);
      in_cs++;
 
      assert(in_cs==1);
      in_cs++;
 
      assert(in_cs==2);
      in_cs++;
  
      assert(in_cs==3);
      in_cs=0;
 
      unlock(val);
   
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

  for(i = 0; i < THREADCOUNT; ++i)
    {
      thr_data[i].tid = i;
      if(pthread_create(&threads[i],NULL, &thread_body, &thr_data[i]))
	{
	  printf("Failed to create threads");
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



