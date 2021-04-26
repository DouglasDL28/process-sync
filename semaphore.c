/* 
Universidad del Valle de Guatemala
Sistemas Operativos
Douglas de León Molina

compilar y correr con con:
    gcc semaphore.c -o sem -lpthread -lrt
    ./semaphore
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/syscall.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define N_THREADS 20
#define N_ITERS 4

int available_resources;
available_resources = 10;

sem_t mutex;
sem_t mutex2;

FILE *file;

/* Pthread method */
void* method() {
    useconds_t sleeptime;
    int thread_id = (int) syscall(SYS_gettid);

    fprintf(file, "Iniciando pthread: %d\n", thread_id);

    for (int i = 0; i < N_ITERS; i++) {
        fprintf(file, "Thread %d - iteración: %d\n", thread_id, i);

        fprintf(file, "%d semáforo abierto.\n",thread_id);
        sem_wait(&mutex2);

        sem_wait(&mutex);
        available_resources = available_resources - 1;
        sem_post(&mutex);

        fprintf(file, "Thread %d - consumiendo recursos\n", thread_id);
        
        sleeptime = (useconds_t) rand() % 1500;
        usleep(sleeptime);
        
        sem_wait(&mutex);
        available_resources = available_resources + 1;
        sem_post(&mutex);

        fprintf(file, "%d recurso usado.\n",thread_id);
        sem_post(&mutex2);
        fprintf(file, "Thread %d - liberando recursos\n", thread_id);
    }

    pthread_exit(0);
}

/* 
Creates N_THEADS p_threads that executes method.
*/
void simulator(void) {
    pthread_t tid[N_THREADS];
    pthread_attr_t attr[N_THREADS];
    
    fprintf(file, "Iniciando procesos\n");

    for (int i = 0; i < N_THREADS; i++) {
        /* get the default attributes */
        pthread_attr_init(&attr[i]);
        /* create thread */
        pthread_create(&tid[i], &attr[i], method, NULL);
    }

    for (int j = 0; j < N_THREADS; j++) {
        /* wait for the thread to exit */
        pthread_join(tid[j], NULL);
    }
}


int main(int argc, char const *argv[])
{
    sem_init(&mutex, 0, 1);
    sem_init(&mutex2, 0, available_resources);

    /* Bitácora */
    file = fopen("bitácora-semáforos.txt", "w");
    if (file == NULL)
    {
        fprintf(file,"Error opening file!\n");
        exit(1);
    }

    simulator();

    sem_destroy(&mutex);
    sem_destroy(&mutex2);

    return 0;
}
