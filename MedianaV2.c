#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#define M 5  // Deve essere dispari
#define N 7  // Numero di colonne

int matrix[M][N];
int medians[M];
int median_of_medians;
sem_t *sem;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int medians_count = 0;

// Funzione di confronto per il quicksort
int cmpfunc(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// Funzione per calcolare la mediana di una riga
void *compute_median(void *arg) {
    int row = *(int *)arg;
    free(arg);

    printf("[Thread %d] Ordinamento riga %d...\n", row, row);
    qsort(matrix[row], N, sizeof(int), cmpfunc);
    int median = matrix[row][N / 2];
    printf("[Thread %d] Mediana calcolata: %d\n", row, median);
    
    pthread_mutex_lock(&mutex);
    medians[medians_count++] = median;
    printf("[Thread %d] Inserita mediana nella posizione %d\n", row, medians_count - 1);
    if (medians_count == M) {
        printf("[Thread %d] Tutte le mediane calcolate, segnale al thread delle mediane\n", row);
        sem_post(sem);
    }
    pthread_mutex_unlock(&mutex);
    
    pthread_cond_signal(&cond);
    return NULL;
}

// Funzione per calcolare la mediana delle mediane
void *compute_median_of_medians(void *arg) {
    printf("[Thread Mediana] In attesa delle mediane...\n");
    sem_wait(sem);
    
    printf("[Thread Mediana] Ordinamento delle mediane...\n");
    qsort(medians, M, sizeof(int), cmpfunc);
    median_of_medians = medians[M / 2];
    
    printf("[Thread Mediana] Mediana delle mediane calcolata: %d\n", median_of_medians);
    
    sem_close(sem);
    sem_unlink("/median_sem");
    return NULL;
}

int main() {
    pthread_t threads[M];
    pthread_t median_thread;
    sem = sem_open("/median_sem", O_CREAT, 0644, 0);

    srand(time(NULL));
    printf("Matrice generata:\n");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % 100;
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < M; i++) {
        int *arg = malloc(sizeof(int));
        *arg = i;
        printf("[Main] Creazione thread per riga %d\n", i);
        pthread_create(&threads[i], NULL, compute_median, arg);
    }
    
    printf("[Main] Creazione del thread per la mediana delle mediane\n");
    pthread_create(&median_thread, NULL, compute_median_of_medians, NULL);
    
    for (int i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
        printf("[Main] Thread per riga %d terminato\n", i);
    }
    pthread_join(median_thread, NULL);
    printf("[Main] Thread della mediana delle mediane terminato\n");
    
    return 0;
}
