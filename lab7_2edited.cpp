#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h> // for usleep()
#include <stdlib.h> // for exit() and random generator
#include <wait.h>	// for wait()
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
// #include "conio.h"
// #include <curses.h>

#define CHAIRNUM 5
#define NUM_TIMES_TO_EAT 10

sem_t chopstick[CHAIRNUM];
int timeUsed[CHAIRNUM];

void randomDelay(void);
void *philosopher(void *who);

int main()
{
	pthread_t tid[CHAIRNUM]; // Child Process ID
	pthread_attr_t attr[CHAIRNUM];
	int param[CHAIRNUM];
	int i;

	for (i = 0; i < CHAIRNUM; i++)
	{
		param[i] = i;
		timeUsed[i] = 0;
		sem_init(&chopstick[i], 0, 1);
	}

	for (i = 0; i < CHAIRNUM; i++)
		pthread_attr_init(&attr[i]);

	for (i = 0; i < CHAIRNUM; i++)
		pthread_create(&tid[i], &attr[i], philosopher, (void *)&param[i]);

	for (i = 0; i < CHAIRNUM; i++)
		pthread_join(tid[i], NULL);

	for (i = 0; i < CHAIRNUM; i++)
		sem_destroy(&chopstick[i]);

	for (i = 0; i < CHAIRNUM; i++)
		printf("timeUsed[%c] = %d\n", i + 'A', timeUsed[i]);

	return 0;
}

void *philosopher(void *who)
{
	int no = (int)*((int *)who);
	int i = 0;

	for (; i < NUM_TIMES_TO_EAT;)
	{
		printf("Mr. %c is thinking...                                    \n", 'A' + no);
		fflush(stdout);
		randomDelay();
		sem_wait(&chopstick[no]);
		printf("Mr. %c is taking a chopstick on the left side...    \n", 'A' + no);
		fflush(stdout);
		randomDelay();
		if (sem_trywait(&chopstick[(no + 1) % CHAIRNUM]) == -1) // Return 0 is success
		{														// Try if the lock is a success
			sem_post(&chopstick[no]);
			continue;
		}
		else
		{
			printf("Mr. %c is taking a chopstick on the right side...\n", 'A' + no);
			fflush(stdout);
		}
		// Critical Section
		randomDelay();
		i++;
		timeUsed[no]++;
		timeUsed[(no + 1) % CHAIRNUM]++;
		randomDelay();
		sem_post(&chopstick[no]);
		printf("Mr. %c drops a chopstick on the left side...        \n", 'A' + no);
		fflush(stdout);
		sem_post(&chopstick[(no + 1) % CHAIRNUM]);
		printf("Mr. %c drops a chopstick on the right side...       \n", 'A' + no);
		fflush(stdout);
		// Remaining Section
		randomDelay();
		printf("Mr. %c is chewing food...                             \n", 'A' + no);
		fflush(stdout);
		randomDelay();
	}
	printf("Mr. %c is full...                                         \n", 'A' + no);
	fflush(stdout);
	pthread_exit(0);
}

void randomDelay(void)
{
	// This function provides a delay which slows the process down so we can see what happens
	srand(time(NULL));
	int stime = ((rand() % 2000) + 100) * 1000;
	usleep(stime);
}