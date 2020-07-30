#ifndef SEM_SHM_FUN
#define SEM_SHM_FUN

int shm_create(char*, size_t, int);
int sem_create(char*, int, int);
int shm_recover(char*, size_t);
int sem_recover(char*);
void sem_delete(char*);
void shm_delete(char*, size_t);

#endif