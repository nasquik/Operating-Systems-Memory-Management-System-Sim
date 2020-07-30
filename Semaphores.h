typedef union semun {               //pre-existing union to help with semaphore functions
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
}semun;

int set_semvalue(int, int, int);
void del_semvalue(int, int);
int semaphore_p(int, int);
int semaphore_v(int, int);