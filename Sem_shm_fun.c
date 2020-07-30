#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include "Sem_shm_fun.h"

// The lab notes were used extensively for the creation of these functions (specifically Lab2, Code 4(Server/Client)) //

/////////// SHM FUNCTIONS /////////////

int shm_create(char* name, size_t size, int value){
    /*Function for  the creation of a shm segment, using
    a file with the specified name, with the specified
    size and using the specified value to create a key*/

    key_t shm_key;
    int shm_id;
    int shm_fd;

    shm_key = ftok(".", value);

    shm_fd = open(name, O_WRONLY | O_TRUNC | O_EXCL | O_CREAT, 0664);
    if(shm_fd < 0){
        perror("Could not open shm key file");
        exit(1);
    }

    if(write(shm_fd, &shm_key, sizeof(key_t)) < 0){
        perror("Could not write key to shm file");
        exit(2);
    }

    close(shm_fd);

    shm_id = shmget(shm_key, size, IPC_CREAT | IPC_EXCL | 0666);
    if(shm_id < 0){
        perror("Could not create shared memory segment");
        unlink(name);
        exit(3);
    }
    return shm_id;
}


int shm_recover(char* name, size_t size){
    /* Function for the recovery of the shm_id of
    a shm segment with the size specified, whose key
    is saved in a file with the specified name*/
    
    int shm_fd;
    int shm_id;
    key_t shm_key;
    
    shm_fd = open(name, O_RDONLY);
    if (shm_fd < 0) {
        perror("Could not open shm key for reading");
        exit(1);
    }

    if (read(shm_fd, &shm_key, sizeof(key_t)) != sizeof(key_t)) {
        perror("Error reading the shm key");
        exit(2);
    }

    close(shm_fd);

    shm_id = shmget(shm_key, size, 0);
    if (shm_id < 0) {
        perror("Could not obtain shared memory segment");
        exit(3);
    }
    return shm_id;
}


void shm_delete(char* name, size_t size){
    /*Function for the deletion of the shm segment
    with the specified size, whose key is saved in 
    a file with the specified name*/

    int shm_id = shm_recover(name, size);

    if(unlink(name) < 0){
        perror("Could not unlink shm key file");
    }

    if(shmctl(shm_id, IPC_RMID, 0) < 0){
        perror("Could not delete shared memory segment");
    }
}


////////////////////////////////////////////////

/////////// SEM FUNCTIONS //////////////

int sem_create(char* name, int number, int value){
    /*Function for  the creation of a semaphore set using
    a file with the specified name, containing the specified
    number of sems and using the specified value to create a key*/
        
    key_t sem_key;
    int sem_fd;
    int sem_id;

    sem_key = ftok(".", value);

    sem_fd = open(name, O_WRONLY | O_TRUNC | O_EXCL | O_CREAT, 0664);
    if(sem_fd < 0){
        perror("Could not open semaphore key file");
        exit(1);
    }

    if(write(sem_fd, &sem_key, sizeof(key_t)) < 0){
        perror("Could not write key to sem file");
        exit(2);
    }

    close(sem_fd);

    sem_id = semget(sem_key, number, IPC_CREAT | IPC_EXCL | 0666);
    if(sem_id < 0){
        perror("Could not create semaphore");
        unlink(name);
        exit(3);
    }
    return sem_id;
}


int sem_recover(char* name){
    /* Function for the recovery of the sem_id of
    a semaphore set whose key is saved in a file 
    with the specified name*/
	
    int sem_fd;
    key_t sem_key;
    int sem_id;

    sem_fd = open(name, O_RDONLY);
    if (sem_fd < 0) {
        perror("Could not open sem key for reading");
        exit(1);
    }

    if(read(sem_fd, &sem_key, sizeof(key_t)) != sizeof(key_t)) {
        perror("Error reading the semaphore key");
        exit(2);
    }

    close(sem_fd);

    sem_id = semget(sem_key, 0, 0);
    if (sem_id < 0) {
        perror("Could not obtain semaphore");
        exit(3);
    }

	return sem_id;
}


void sem_delete(char* name){
    /*Function for the deletion of the
    semaphore set whose key is saved in 
    a file with the specified name*/

    int sem_id = sem_recover(name);
        
    if(unlink(name) < 0){
        perror("Could not unlink sem key file");
    }

    if(semctl(sem_id, 0, IPC_RMID) < 0){
        perror("Could not delete semaphore");
    }
}