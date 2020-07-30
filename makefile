default: Fabrication

Main.o: Main.c keys.h Semaphores.h Sem_shm_fun.h Component.h Manufacturing.h DynamicQueue.h
	gcc -c Main.c -o Main.o

Manufacturing.o: Manufacturing.c Semaphores.h Sem_shm_fun.h Component.h DynamicQueue.h
	gcc -c Manufacturing.c -o Manufacturing.o

DynamicQueue.o: DynamicQueue.c Component.h DynamicQueue.h
	gcc -c DynamicQueue.c -o DynamicQueue.o

Component.o: Component.c Component.h
	gcc -c Component.c -o Component.o

Semaphores.o: Semaphores.c Semaphores.h
	gcc -c Semaphores.c -o Semaphores.o

Sem_shm_fun.o: Sem_shm_fun.c Sem_shm_fun.h
	gcc -c Sem_shm_fun.c -o Sem_shm_fun.o

Fabrication: Main.o Manufacturing.o DynamicQueue.o Component.o Semaphores.o Sem_shm_fun.o
	gcc Main.o Manufacturing.o DynamicQueue.o Component.o Semaphores.o Sem_shm_fun.o -o Fabrication

clean:
	-rm -f Fabrication Main.o Manufacturing.o DynamicQueue.o Component.o Semaphores.o Sem_shm_fun.o *.key
