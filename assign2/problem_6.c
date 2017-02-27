#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
volatile unsigned long incircle;
volatile unsigned long total;
volatile int done;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/*
  Generates a random number between -1 & 1
 */
double randNumGen(double min, double max)
{
  unsigned int r = rand();
  return (rand_r(&r)/(double)(RAND_MAX))*abs(min-max)+min;
}


void *thread_body()
{
  unsigned long thread_count = 0;
  unsigned long i;
  for( i = 0; !done; i++)
    {
      double x = randNumGen(-1,1);
      double y = randNumGen(-1,1);
      
       if((x*x)+(y*y) < 1)
	{
	  thread_count++;
	}
      
    }
 
  pthread_mutex_lock(&mutex);
  incircle += thread_count;
  total+=i;
  pthread_mutex_unlock(&mutex);
  pthread_exit(NULL);
}





int main(int argc,char **argv)
{
   if(argc != 3){
    printf("Usage: ./a.out <Num Threads> <Time in Sec>");
    return 0;
  }
 
  int THREADCOUNT = atoi(argv[1]);
  int sec = atoi(argv[2]);
  pthread_t threads[THREADCOUNT];
  int i;
  incircle = 0;
  total = 0;
  for(i = 0; i < THREADCOUNT; ++i)
    {
      if(pthread_create(&threads[i],NULL, &thread_body,(void *)NULL))
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

  double pi = 4*((double)incircle/(double)total);
  printf("This pi %f\n",pi);
  return 0;
  

}
