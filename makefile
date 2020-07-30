default: Project

main.o : main.c keys.h Semaphores.h Sem_shm_fun.h list.h hashTable.h functions.h
	gcc -g -c main.c -o main.o

functions.o : functions.c keys.h Semaphores.h Sem_shm_fun.h list.h hashTable.h
	gcc -g -c functions.c -o functions.o

hashTable.o : hashTable.c list.h hashTable.h
	gcc -g -c hashTable.c -o hashTable.o

list.o : list.c list.h
	gcc -g -c list.c -o list.o

Semaphores.o: Semaphores.c Semaphores.h
	gcc -g -c Semaphores.c -o Semaphores.o

Sem_shm_fun.o: Sem_shm_fun.c Sem_shm_fun.h
	gcc -g -c Sem_shm_fun.c -o Sem_shm_fun.o

Project: main.o functions.o hashTable.o list.o Sem_shm_fun.o Semaphores.o
	gcc main.o functions.o hashTable.o list.o Sem_shm_fun.o Semaphores.o -g -o Project

clean:
	-rm -f Project *.o *.key
