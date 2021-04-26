/* 
Universidad del Valle de Guatemala
Sistemas Operativos
Douglas de León Molina

compilar y correr con con:
    gcc monitor.c -o monitor -lpthread -lrt
    ./monitor
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/syscall.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define N_THREADS 10
#define N_ITERS 3

int available_resources;
available_resources = 20;

sem_t mutex;
sem_t mutex2;

FILE *file;


/* decrease available resources by count resources */
/* return 0 if sufficient resources available, */
/* otherwise return -1 */
int decrease_count(int count) {
    sem_wait(&mutex);
    fprintf(file,"Entrando al monitor\n");
    if (available_resources < count) {
        sem_post(&mutex);
        return -1;
    }

    else {
        fprintf(file,"Recursos suficientes, consumiendo...\n");
        for (int i = 0; i < count; i++) // sumamos count a semáforo
            sem_post(&mutex2);
        
        available_resources -= count;
        sem_post(&mutex);
        return 0;
    }
}


/* increase available resources by count */
int increase_count(int count) {
    sem_wait(&mutex);
    available_resources += count;
    sem_post(&mutex);
    return 0;
}


/* Pthread method */
void* method() {
    useconds_t sleeptime;
    int thread_id = (int) syscall(SYS_gettid);

    fprintf(file, "Iniciando pthread: %d\n", thread_id);
    
    for (int i = 0; i < N_ITERS; i++) {

        fprintf(file, "Thread %d - iteración: %d\n", thread_id, i);

        int res;
        res = decrease_count(10);
        if (res < 0) {
            fprintf(file,"No hay suficientes recursos\n");
        } else {
            fprintf(file,"Se consumirán 10 recursos");
        }

        sem_wait(&mutex2);
        fprintf(file, "%d recursos disponibles.\n", available_resources);
        sleeptime = (useconds_t) rand() % 1500;
        usleep(sleeptime);

        increase_count(10);

    }

    pthread_exit(0);
}

/* 
Creates N_THEADS p_threads that executes method.
*/
void simulator(void) {
    pthread_t tid[N_THREADS];
    pthread_attr_t attr[N_THREADS];
    
    fprintf(file,"Iniciando procesos\n");

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


int main(int argc, char const *argv[]){
    sem_init(&mutex, 0, 1);
    sem_init(&mutex2, 0, 0);

    file = fopen("bitácora-monitor.txt", "w");
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

