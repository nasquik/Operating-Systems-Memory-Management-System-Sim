#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <fcntl.h>
#include "Semaphores.h"

// These functions were taken directly from notes from Lab2, Code 3 //

int set_semvalue(int sem_id, int num, int value)
{
	semun sem_union;
	sem_union.val = value;
	if (semctl(sem_id, num, SETVAL, sem_union) == -1)
        return(0);
	return(1);
}

void del_semvalue(int sem_id, int num)
{
	semun sem_union;
	if (semctl(sem_id, num, IPC_RMID, sem_union) == -1)
	    fprintf(stderr, "Failed to delete semaphore\n");
}

int semaphore_p(int sem_id, int num)
{
	struct sembuf sem_b;
	sem_b.sem_num = num;
	sem_b.sem_op = -1; /* P() */
	sem_b.sem_flg = 0;
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "semaphore_p failed\n");
		return(0);
	}
	return(1);
}

int semaphore_v(int sem_id, int num)
{
	struct sembuf sem_b;
	sem_b.sem_num = num;
	sem_b.sem_op = 1; /* V() */
	sem_b.sem_flg = 0;
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "semaphore_v failed\n");
		return(0);
	}
	return(1);
}