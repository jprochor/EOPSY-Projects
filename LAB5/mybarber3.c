
#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#include <unistd.h>

#include <sys/ipc.h>

#include <sys/sem.h>

#include <sys/shm.h>

#include <time.h>

#define FEMALECUSTOMERS 0

#define MALECUSTOMERS 0

#define FEMALEBARBERS_N1 3

#define MALEBARBERS_N2 1

#define BOTHBARBERS_N3 1

#define MUTEX 2

#define CHAIRS 4

//semaphore functions definitions

void up(int sem_id,int sem_num,struct sembuf *semaphore) {

  semaphore->sem_num=sem_num;

  semaphore->sem_op=1;

  semaphore->sem_flg=0;

  semop(sem_id,semaphore,1);

}

void down(int sem_id,int sem_num,struct sembuf *semaphore) {

  semaphore->sem_num=sem_num;

  semaphore->sem_op=-1;

  semaphore->sem_flg=0;

  semop(sem_id,semaphore,1);

}

void initSem(int sem_id,int sem_num,int val) {

  union semnum {

    int val;

    struct semid_ds *buf;

    unsigned short *array;

  }argument;

  argument.val=val;

  semctl(sem_id,sem_num,SETVAL,argument);

}



int main() {

  int sem_key=1111,shm_key=2222,sem_key2=3333,shm_key2=4444,sem_key3=4444,shm_key3=5555,sem_key4=6666,shm_key4=7777,sem_key5=8888,shm_key5=9999,sem_key6=11111,shm_key6=22222;

  int sem_id,shm_id,sem_id2,shm_id2,sem_id3,shm_id3,sem_id4,shm_id4,sem_id5,shm_id5,sem_id6,shm_id6;

  int *waiting;
int *femalewaiting;
int *malewaiting;
int *freefemalebarbers;
int *freemalebarbers;
int *freebothbarbers;

  struct sembuf semaphore;

int count=1;
int count2=1;
int count3=1;
int count4=1;


//allocating shared memory
shm_id=shmget(shm_key,sizeof(int),IPC_CREAT|0666);
sem_id=semget(sem_key,3,IPC_CREAT|0666);
waiting=shmat(shm_id,NULL,0);
*waiting=0;

femalewaiting=waiting+1;
*femalewaiting=0;

malewaiting=waiting+2;
*malewaiting=0;

freefemalebarbers=waiting+3;
*freefemalebarbers=FEMALEBARBERS_N1;

freemalebarbers=waiting+4;
*freemalebarbers=MALEBARBERS_N2;

freebothbarbers=waiting+5;
*freebothbarbers=BOTHBARBERS_N3;





//initializing semaphores
  initSem(sem_id,FEMALECUSTOMERS,0);
  initSem(sem_id,MALECUSTOMERS,0);

  initSem(sem_id,FEMALEBARBERS_N1,0);
  initSem(sem_id,MALEBARBERS_N2,0);
  initSem(sem_id,BOTHBARBERS_N3,0);

  initSem(sem_id,MUTEX,1);

  printf("There are %d chairs.\n",CHAIRS);

//forks for barbers
int femalebarber=fork();
int malebarber=fork();
int bothbarber=fork();



if (femalebarber==0)  //female barber part
{

    while(1) {

	srand(time(NULL));
	int RAND= rand()%10; //generating random time interval for cutting hair/sleeping
if(*femalewaiting!=0 && *freefemalebarbers!=0) // if there is a female client waiting and there is a free female barber
	{
	*freefemalebarbers=*freefemalebarbers-1; //one female barber is now busy
	*femalewaiting=*femalewaiting-1;  //one female client taken away from the queue

      	down(sem_id,FEMALECUSTOMERS,&semaphore); //semaphore waiting for customer for synchronization

      	down(sem_id,MUTEX,&semaphore); //chair mutex lock

      	*waiting=*waiting-1;  //one less person sitting on a chair in waiting room

      	up(sem_id,FEMALEBARBERS_N1,&semaphore);  //bringing customer for haircut

      	up(sem_id,MUTEX,&semaphore); //chair mutex free

      	printf("The female barber is now cutting hair.\n");

     	sleep(RAND);  // cut hair for random time 
	*freefemalebarbers=*freefemalebarbers+1; //female barber is free again afterwards
	}
else // no proper client found or no free barbers
{
sleep(RAND); // just take a quick nap
}

    	}

  }



//male barber part
if (malebarber==0)
{

	while(1) {
	srand(time(NULL));
	int RAND= rand()%10;
if(*malewaiting!=0 && *freemalebarbers!=0)
	{
	*freemalebarbers=*freemalebarbers-1;
	*malewaiting=*malewaiting-1;
	down(sem_id,MALECUSTOMERS,&semaphore);

      	down(sem_id,MUTEX,&semaphore);

      	*waiting=*waiting-1;

      	up(sem_id,MALEBARBERS_N2,&semaphore);

      	up(sem_id,MUTEX,&semaphore);

      	printf("The male barber is now cutting hair.\n");

      	sleep(RAND);  
	*freemalebarbers=*freemalebarbers+1; 
	}
	else
	{
	sleep(RAND);
	}

}
}


//both barber(cuts both male and female hair) part
if (bothbarber==0)
{

	while(1) {
	srand(time(NULL));
	int RAND= rand()%10;
if(*malewaiting!=0 && *freebothbarbers!=0) //male waiting
	{
	*freebothbarbers=*freebothbarbers-1;
	*malewaiting=*malewaiting-1;
	down(sem_id,MALECUSTOMERS,&semaphore);

      	down(sem_id,MUTEX,&semaphore);

      	*waiting=*waiting-1;

      	up(sem_id,MALEBARBERS_N2,&semaphore);

      	up(sem_id,MUTEX,&semaphore);

      	printf("The both barber is now cutting male hair.\n");

      	sleep(RAND);  
	*freebothbarbers=*freebothbarbers+1; 
	}

else if(*femalewaiting!=0 && *freebothbarbers!=0) //female waiting
	{
	*freebothbarbers=*freebothbarbers-1;
	*femalewaiting=*femalewaiting-1;
      	down(sem_id,FEMALECUSTOMERS,&semaphore);

      	down(sem_id,MUTEX,&semaphore);

      	*waiting=*waiting-1;

      	up(sem_id,FEMALEBARBERS_N1,&semaphore);

      	up(sem_id,MUTEX,&semaphore);

      	printf("The both barber is now cutting female hair.\n");

      	sleep(RAND);  
	*freebothbarbers=*freebothbarbers+1; 
	}
	else
	{
	sleep(RAND);
	}


}
}











//customer routine
if(femalebarber>0 || malebarber>0 || bothbarber>0)
{

  

    while(1) {

      sleep(1);   //one customer comes into barber shop once every second

      down(sem_id,MUTEX,&semaphore);

      if(*waiting < CHAIRS) //if there are still free chairs in the waiting room
	{ 
	srand(time(NULL));
	int RAND2= rand()%10; //generate random number to decide whether male or female enters barber shop
		if(RAND2>5)
		{
		*femalewaiting=*femalewaiting+1; //one extra female is waiting
      		printf("Female Customer %d is seated.\n",count3++);
		count++;

      		*waiting=*waiting+1; //one more chair has been taken in waiting room

     		up(sem_id,FEMALECUSTOMERS,&semaphore); //notify barber

      		up(sem_id,MUTEX,&semaphore);  //free chair lock
		}
		else
		{
		*malewaiting=*malewaiting+1; //one extra male is waiting
		printf("Male Customer %d is seated.\n",count4++);
		count2++;

      		*waiting=*waiting+1; //one more chair has been taken in waiting room

      		up(sem_id,MALECUSTOMERS,&semaphore); //notify barber
 
      		up(sem_id,MUTEX,&semaphore); //free chair lock

		}

      }

      else //turns out there were no free chairs in waiting room
	{
	srand(time(NULL));
	int RAND3= rand()%10; //generate random number to decided whether it was male or female who tried to enter barber shop
		if(RAND3>5)
		{

      		printf("Female Customer %d left the shop because there were no chairs in the waiting room.\n",count++);

      		up(sem_id,MUTEX,&semaphore); //free chair lock
		}
		else
		{
		printf("Male Customer %d left the shop because there were no chairs in the waiting room.\n",count2++);

      		up(sem_id,MUTEX,&semaphore); //free chair lock

		}

      	}

    	}

  }

}

