#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

typedef enum
  {
    bird, dog, cat
  
  } pet;

typedef struct thread_data
{
  int tid;
  pet type;
  int count;
  pthread_mutex_t* lock;
} thread_data;

volatile int playing = 1;
volatile int cats = 0;
volatile int dogs = 0;
volatile int birds = 0;
volatile int n_cats = 0;
volatile int n_dogs = 0;
volatile int n_birds = 0;


pthread_cond_t* bird_cv;
pthread_cond_t* cat_cv;
pthread_cond_t* dog_cv;

void cat_enter( thread_data* data){

  pthread_mutex_lock(data->lock);
  //wait while there are birds and dogs playing
  while(birds!=0 || dogs!=0)
    {
      pthread_cond_wait(cat_cv, data->lock);
    }
  cats++;
  data->count++;
  pthread_mutex_unlock(data->lock);
}

void cat_exit(thread_data* data){
  
  pthread_mutex_lock(data->lock);
  
  cats--;
  //Tell the dogs and birds they can play
  if(cats==0)
    {
      pthread_cond_signal(bird_cv);
      pthread_cond_signal(dog_cv);
    }
  
  pthread_mutex_unlock(data->lock);
}


void dog_enter(thread_data* data){
  pthread_mutex_lock(data->lock);
  //dog can play when no cats are playing
  while(cats!=0)
    {
      pthread_cond_wait(dog_cv, data->lock);
    }
  dogs++;

  data->count++;
  pthread_mutex_unlock(data->lock);
}

void dog_exit(thread_data* data){
  
  pthread_mutex_lock(data->lock);
  dogs--;
  //cats can play only when dogs and birds are not playing
  if(birds==0 && dogs==0)
    {
      pthread_cond_signal(cat_cv);
    }
  pthread_mutex_unlock(data->lock);
}

void bird_enter(thread_data* data){
  
  pthread_mutex_lock(data->lock);
  //birds play when no cats
  while(cats!=0)
    {
      pthread_cond_wait(bird_cv, data->lock);
    }
  birds++;
  data->count++;
  pthread_mutex_unlock(data->lock);
}

void bird_exit(thread_data* data){
  
  pthread_mutex_lock(data->lock);
  birds--;
  //cats can play when there are no birds or dogs
  if(birds==0 && dogs==0)
    {
      pthread_cond_signal(cat_cv);
    }
  pthread_mutex_unlock(data->lock);
}

void play(void) {
  int i;
  for (i=0; i<10; i++) {
    assert(cats >= 0 && cats <= n_cats);
    assert(dogs >= 0 && dogs <= n_dogs);
    assert(birds >= 0 && birds <= n_birds);
    assert(cats == 0 || dogs == 0);
    assert(cats == 0 || birds == 0);
  }
}

void* pet_hotel(void* arg)
{
  thread_data* data = ((thread_data* )arg);
  
  if(data->type == cat)
    {
      while(playing)
  {
    cat_enter(data);
    play();
    cat_exit(data);
  }
    }

  if(data->type == bird)
    {
      while(playing)
  {
    bird_enter(data);
    play();
    bird_exit(data);
  }
    }
  
  
  if(data->type == dog)
    {
      while(playing)
  {
    dog_enter(data);
    play();
    dog_exit(data);
  }
    }
  
  return NULL;
} 

int main(int argc, char* argv[]){
  if(argc < 4){
    printf("Missing Arguments! Expected file_name # # #");
    return -1;
  }
  n_cats = atoi(argv[1]);
  n_dogs = atoi(argv[2]);
  n_birds = atoi(argv[3]);

  if((n_cats < 0 && n_cats > 99) || (n_dogs < 0 && n_dogs > 99) || (n_birds < 0 && n_birds > 99)){
    printf("Invalid Arguments! Number of pets need to be between 0 and 99 (inclusive)");
  }

  int total = n_cats+n_dogs+n_birds;
  pthread_t threads[total];
  thread_data data[total];


  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t c_birds;
  bird_cv = &c_birds;
  pthread_cond_t c_dogs;
  dog_cv = &c_dogs;
  pthread_cond_t c_cats;
  cat_cv = &c_cats;

  pthread_cond_init (bird_cv, NULL);
  pthread_cond_init (dog_cv, NULL);
  pthread_cond_init (cat_cv, NULL);
  

  int i;

  for(i = 0; i < total; i++)
    {
      data[i].tid = i;
      data[i].lock = &lock;

      if(i < n_cats)
  data[i].type = cat;
      else if(i >= n_cats && i < (total- n_birds))
  data[i].type = dog;
      else
  data[i].type = bird;
      data[i].count = 0;

      int fail = pthread_create(&threads[i],NULL, pet_hotel, &data[i]);
      if(fail)
  {
    printf("Failed to create threads\n");
    return -1;
  }
    }

  sleep(10);
  playing = 0;

  for(i=0; i < total; i++)
    {
      pthread_join(threads[i], NULL);
    }

  int cat_total=0;
  int bird_total=0;
  int dog_total=0;
  for(i=0; i < total; i++)
    {
      if(i < n_cats)
  cat_total+=data[i].count;
      else if(i >= n_cats && i < (total-n_birds))
  dog_total+=data[i].count;
      else
  bird_total+=data[i].count;
    }

  printf("cat played = %i, dog played = %i, bird played = %i\n", cat_total, dog_total, bird_total);

  return 1;

}
