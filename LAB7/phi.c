#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define N 5
#define LEFT (num + N - 1) % N
#define RIGHT (num + 1) % N

pthread_mutex_t m;   //global singular mutex
pthread_cond_t condition[N];  //conditional mutex for each philosopher

enum { THINKING, HUNGRY, EATING } state[N];  // three possible states
int phil_num[N];  //identifies the philosopher

void *philosophing (void *arg);  //routine of philosophers
void grab_forks(int num);   //function for trying to grab forks
void put_away_forks(int num);  //function for putting away forks
void test(int num);  //testing if left and right neighbour are thinking/hungry etc

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
    printf("Philosopher #%d sits himself around the table.\n", i + 1); //print info about start of philosopher
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
    int *num = arg;
    printf("Philosopher #%d is thinking.\n", *num + 1); //print info that philosopher started thinking
    sleep(2);
    grab_forks(*num);  //try to grab forks
    put_away_forks(*num);  //put away forks after done with eating
  }
}

void grab_forks(int num)  //grabbing forks if it's allowed (critical region)
 {
  pthread_mutex_lock(&m);  //lock mutex

  printf("Philosopher #%d is hungry and tries to grab the forks\n", num + 1);
  state[num] = HUNGRY;  //now he signals being hungry
  test(num); //test neighbor situation
  while (state[num] != EATING) 
    pthread_cond_wait(&condition[num], &m);

  pthread_mutex_unlock(&m); //unlock mutex
}

void put_away_forks(int num) //returning forks after meal (critical region)
{
  pthread_mutex_lock(&m);  //lock mutex

  printf("Philosopher #%d puts down forks and asks neighbors if they are hungry.\n", num + 1);
  state[num] = THINKING; //stopped eating now thinks
  test(LEFT);
  test(RIGHT);

  pthread_mutex_unlock(&m); //unlock mutex
}

void test(int num) //checking if given a situation of current neighbours it is possible to eat
{
//if philosopher is hungry and neither of neighbors is eating atm
  if (state[num] == HUNGRY && 
      state[LEFT] != EATING && 
      state[RIGHT] != EATING) {
    printf("Philosopher #%d starts eating.\n", num + 1); // start eating
    state[num] = EATING;
    sleep(3);
    pthread_cond_signal(&condition[num]);
  }
}


