#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 4
#define NUMTHREAD 2

pthread_mutex_t mutexEstrazione;
pthread_cond_t condFine;

int completed = 0;
int **M, *v1, *v2; /* M = matrice, v1 = vettore diagonale primaria, v2 = vettore diagonale secondaria */

void* estrazione(void* arg) {
    int threadId = *(int*)arg;
    
    if (threadId%2==0) {
        // Estrarre la diagonale principale
        for (int i = 0; i < N; i++) {
            v1[i] = M[i][i];
        }
        printf("Diagonale principale: ");
        for (int i = 0; i < N; i++) {
            printf("%d ", v1[i]);
        }
        printf("\n");
    } else {
        // Estrarre la diagonale secondaria
        for (int i = 0; i < N; i++) {
            v2[i] = M[i][N - 1 - i];
        }
        printf("Diagonale secondaria: ");
        for (int i = 0; i < N; i++) {
            printf("%d ", v2[i]);
        }
        printf("\n");
    }

    pthread_mutex_lock(&mutexEstrazione);
    completed++;
    if (completed == NUMTHREAD) {
        pthread_cond_signal(&condFine);
    }
    pthread_mutex_unlock(&mutexEstrazione);

    free(arg); // Libera la memoria allocata per l'argomento
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL)); // Inizializza il generatore di numeri casuali

    pthread_t th[NUMTHREAD];
    pthread_mutex_init(&mutexEstrazione, NULL);
    pthread_cond_init(&condFine, NULL);

    // Allocazione matrice e vettori
    M = (int**)calloc(N, sizeof(int*));
    for (int i = 0; i < N; i++) {
        M[i] = (int*)calloc(N, sizeof(int));
    }

    v1 = (int*)calloc(N, sizeof(int));
    v2 = (int*)calloc(N, sizeof(int));

    // Inizializzazione matrice
    printf("Matrice:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            M[i][j] = 1 + rand() % 10;
            printf("%d ", M[i][j]);
        }
        printf("\n");
    }

    printf("--------------------------------------\n");

    // Creazione dei thread
    for (int i = 0; i < NUMTHREAD; i++) {
        int *a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&th[i], NULL, &estrazione, a) != 0) {
            perror("Create() error");
            exit(EXIT_FAILURE);
        }
    }

    pthread_mutex_lock(&mutexEstrazione);
    while (completed < NUMTHREAD) {
        pthread_cond_wait(&condFine, &mutexEstrazione);
    }
    pthread_mutex_unlock(&mutexEstrazione);

    // Calcolo dei massimi
    int maxD1 = v1[0];
    int maxD2 = v2[0];

    for (int i = 1; i < N; i++) {
        if (v1[i] > maxD1) {
            maxD1 = v1[i];
        }
        if (v2[i] > maxD2) {
            maxD2 = v2[i];
        }
    }

    printf("Valore massimo della diagonale principale: %d\n", maxD1);
    printf("Valore massimo della diagonale secondaria: %d\n", maxD2);

    // Deallocazione memoria
    for (int i = 0; i < N; i++) {
        free(M[i]);
    }
    free(M);
    free(v1);
    free(v2);

    pthread_cond_destroy(&condFine);
    pthread_mutex_destroy(&mutexEstrazione);

    return 0;
}
