#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define N 5
#define LEFT (i + N - 1) % N
#define RIGHT (i + 1) % N

pthread_mutex_t m;   //global singular mutex
pthread_cond_t condition[N];  //conditional mutex for each philosopher

enum { THINKING, HUNGRY, EATING } state[N];  // three possible states
int phil_num[N];  //identifies the philosopher

void *philosophing (void *arg);
void grab_forks(int i);
void put_away_forks(int i);
void test(int i);

int main(int argc, char *argv[])
{
  //initialization of mutexes(separate threads)
  pthread_t ph_thread[N];
  pthread_mutex_init(&m, NULL);

  for (int i = 0; i < N; i++) {
    pthread_cond_init(&condition[i], NULL);
    phil_num[i] = i;
  }

  //creation of mutexes(separate threads)
  for (int i = 0; i < N; i++) {
    pthread_create(&ph_thread[i], NULL, philosophing, &phil_num[i]);
    printf("Philosopher #%d sits himself around the table.\n", i + 1);
    sleep(1);
  }

	//joining threads
  for (int i = 0; i < N; i++)
    pthread_join(ph_thread[i], NULL);

  pthread_mutex_destroy(&m);
  for (int i = 0; i < N; i++)
    pthread_cond_destroy(&condition[i]);  //destruction afterwards

  return(0);
}

void *philosophing(void *arg)  //routine of philosopher - starts thinking, then tries to grab forks and eat
{
  while(1) {
    int *i = arg;
    printf("Philosopher #%d is thinking.\n", *i + 1);
    sleep(2);
    grab_forks(*i);
    put_away_forks(*i);
  }
}

void grab_forks(int i)  //grabbing forks if it's allowed (critical region)
 {
  pthread_mutex_lock(&m);

  printf("Philosopher #%d is hungry and tries to grab the forks\n", i + 1);
  state[i] = HUNGRY;
  test(i);
  while (state[i] != EATING) 
    pthread_cond_wait(&condition[i], &m);

  pthread_mutex_unlock(&m);
}

void put_away_forks(int i) //returning forks after meal (critical region)
{
  pthread_mutex_lock(&m);

  printf("Philosopher #%d puts down forks and asks neighbors if they are hungry.\n", i + 1);
  state[i] = THINKING;
  test(LEFT);
  test(RIGHT);

  pthread_mutex_unlock(&m);
}

void test(int i) //checking if given a situation of current neighbours it is possible to eat
{
  if (state[i] == HUNGRY && 
      state[LEFT] != EATING && 
      state[RIGHT] != EATING) {
    printf("Philosopher #%d starts eating.\n", i + 1);
    state[i] = EATING;
    sleep(3);
    pthread_cond_signal(&condition[i]);
  }
}


