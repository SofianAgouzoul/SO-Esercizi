#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 3  // Numero di righe della matrice
#define M 3  // Numero di colonne della matrice
#define NUMT 3 // Numero di thread

int A[N][M] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };
int B[N][M] = { {9, 8, 7}, {6, 5, 4}, {3, 2, 1} };
int AT[M][N], BT[M][N], C[M][N];

sem_t sem_traspostaA, sem_traspostaB;
sem_t sem_somma;
sem_t sem_stampa;

void* trasponiA(void* arg) {
    int id = *(int*)arg;
    for (int i = id; i < N; i += NUMT) {
        for (int j = 0; j < M; j++) {
            AT[j][i] = A[i][j];
        }
    }
    sem_post(&sem_traspostaA);
    return NULL;
}

void* trasponiB(void* arg) {
    int id = *(int*)arg;
    for (int i = id; i < N; i += NUMT) {
        for (int j = 0; j < M; j++) {
            BT[j][i] = B[i][j];
        }
    }
    sem_post(&sem_traspostaB);
    return NULL;
}

void* somma(void* arg) {
    sem_wait(&sem_traspostaA);
    sem_wait(&sem_traspostaB);

    int id = *(int*)arg;
    for (int i = id; i < M; i += NUMT) {
        for (int j = 0; j < N; j++) {
            C[i][j] = AT[i][j] + BT[i][j];
        }
    }
    sem_post(&sem_somma);
    return NULL;
}

void* stampa(void* arg) {
    sem_wait(&sem_somma);

    printf("Matrice somma C:\n");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }
    return NULL;
}

int main() {
    pthread_t threads[NUMT * 2 + 2]; // NUMT per trasporre A, NUMT per trasporre B, NUMT per somma, 1 per stampa
    int thread_id[NUMT];

    // Inizializzazione semafori
    sem_init(&sem_traspostaA, 0, 0);
    sem_init(&sem_traspostaB, 0, 0);
    sem_init(&sem_somma, 0, 0);
    sem_init(&sem_stampa, 0, 0);

    // Creazione thread per trasporre le matrici
    for (int i = 0; i < NUMT; i++) {
        thread_id[i] = i;
        pthread_create(&threads[i], NULL, trasponiA, &thread_id[i]);
        pthread_create(&threads[i + NUMT], NULL, trasponiB, &thread_id[i]);
    }

    // Creazione thread per la somma
    for (int i = 0; i < NUMT; i++) {
        pthread_create(&threads[2 * NUMT + i], NULL, somma, &thread_id[i]);
    }

    // Creazione thread per la stampa
    pthread_create(&threads[2 * NUMT + NUMT], NULL, stampa, NULL);

    // Join dei thread
    for (int i = 0; i < 2 * NUMT + NUMT + 1; i++) {
        pthread_join(threads[i], NULL);
    }

    // Distruzione dei semafori
    sem_destroy(&sem_traspostaA);
    sem_destroy(&sem_traspostaB);
    sem_destroy(&sem_somma);
    sem_destroy(&sem_stampa);

    return 0;
}
