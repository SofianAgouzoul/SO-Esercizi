#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define N 5 // NUMERO DI RIGHE
#define M 7 // NUMERO DI COLONNE
#define T 5 // Numero di threads per calcolare la mediana di ogni riga

int **matrice, *vettore, mediana_delle_mediane, mediansCount = 0;
sem_t *sem;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int cmpfunc(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void* ricercaMediana(void* arg) {
    int row = *(int*)arg;

    // Ordinamento della riga i-esima
    printf("[THREAD %d] - Ordinamento riga %d...\n", row, row);
    qsort(matrice[row], M, sizeof(int), cmpfunc);
    int median = matrice[row][M / 2];  // Calcola la mediana della riga
    printf("[THREAD %d] - Mediana calcolata: %d\n", row, median);

    pthread_mutex_lock(&mutex);
    vettore[mediansCount++] = median;

    if (mediansCount == N) {
        pthread_cond_signal(&cond); // Svegliamo il thread finale quando tutte le mediane sono pronte
    }

    pthread_mutex_unlock(&mutex);

    free(arg);
    pthread_exit(NULL);
}

void* medianaFinale(void* arg) {
    // Aspettiamo che tutte le mediane siano calcolate, usando la variabile di condizione
    pthread_mutex_lock(&mutex);
    while (mediansCount < N) {
        pthread_cond_wait(&cond, &mutex);  // Aspetta che tutte le mediane siano calcolate
    }
    pthread_mutex_unlock(&mutex);

    // Ordinamento del vettore delle mediane
    qsort(vettore, N, sizeof(int), cmpfunc);
    mediana_delle_mediane = vettore[N / 2];  // Calcola la mediana delle mediane
    printf("Mediana delle mediane calcolata: %d\n", mediana_delle_mediane);

    // Dopo che tutte le mediane sono calcolate, postiamo il semaforo per indicare che il lavoro è finito
    sem_post(sem);

    free(arg);
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    int i, j;
    pthread_t th[T], t_stampa;

    sem = sem_open("/medianSem", O_CREAT, 0644, 0);  // Semaforo inizializzato a 0
    if (sem == SEM_FAILED) {
        perror("Errore apertura semaforo");
        exit(EXIT_FAILURE);
    }

    // Allocazione della matrice e del vettore delle mediane
    matrice = (int**)calloc(N, sizeof(int*));
    for (i = 0; i < N; i++) {
        matrice[i] = (int*)calloc(M, sizeof(int));
    }

    vettore = (int*)calloc(N, sizeof(int));  // Alloca N elementi per le mediane

    // Inizializzazione della matrice con numeri casuali
    printf("MATRICE\n");
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            matrice[i][j] = 1 + rand() % 100;  // Genera numeri casuali tra 1 e 100
            printf("%d ", matrice[i][j]);
        }
        printf("\n");
    }

    // Creazione dei thread per calcolare la mediana di ciascuna riga
    for (i = 0; i < T; i++) {
        int *index = (int*)malloc(sizeof(int));
        *index = i;
        if (pthread_create(&th[i], NULL, &ricercaMediana, (void*)index) != 0) {
            perror("Errore nella creazione dei threads\n");
            exit(EXIT_FAILURE);
        }
    }

    // Creazione del thread che calcola la mediana delle mediane
    if (pthread_create(&t_stampa, NULL, &medianaFinale, NULL) != 0) {
        perror("Errore nella creazione del thread stampa\n");
        exit(EXIT_FAILURE);
    }

    // Attendi la terminazione di tutti i thread
    for (i = 0; i < T; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Errore nella join dei threads\n");
            exit(EXIT_FAILURE);
        }
    }

    // Attendi che il thread finale completi la sua esecuzione
    sem_wait(sem);

    if (pthread_join(t_stampa, NULL) != 0) {
        perror("Errore nella join del thread stampa\n");
        exit(EXIT_FAILURE);
    }

    // Liberazione della memoria allocata
    for (i = 0; i < N; i++) {
        free(matrice[i]);
    }
    free(matrice);
    free(vettore);

    // Distruzione dei semafori, mutex e variabili di condizione
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
