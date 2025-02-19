#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define N 4   /* Dimensione della matrice */
#define NUMT 4 /* Numero di thread */

char **matrice, **vettore, carattere;
int count = 0, completed = 0;

sem_t sem;
pthread_mutex_t mutex;
pthread_cond_t cond;

void* ricerca(void* arg) {
    int index = *(int*)arg;

    sem_wait(&sem);  // Attesa del semaforo per accedere alla matrice
    for (int j = 0; j < N; j++) {
        if (matrice[index][j] == carattere) {
            pthread_mutex_lock(&mutex);
            count++;
            pthread_mutex_unlock(&mutex);
            vettore[index][j] = carattere;  // Memorizzo il carattere
        } else {
            vettore[index][j] = '-';  // Se non c'è, metto '-'
        }
    }
    sem_post(&sem);  // Rilascio del semaforo

    pthread_mutex_lock(&mutex);
    completed++;
    if (completed == NUMT) {
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutex);

    free(arg);  // Libero subito la memoria allocata per l'indice
    pthread_exit(NULL);
}

void* stampa(void* arg) {
    pthread_mutex_lock(&mutex);
    while (completed < NUMT) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    printf("Numero di occorrenze del carattere nella matrice: %d\n", count);
    printf("Vettore occorrenze:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%c ", vettore[i][j]);
        }
        printf("\n");
    }

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    pthread_t th[NUMT], t_stampa;
    sem_init(&sem, 0, 1);  // Inizializzazione del semaforo
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    printf("Inserisci il carattere da ricercare: ");
    scanf(" %c", &carattere);  // Spazio prima di %c per evitare problemi con \n

    /* Allocazione della matrice e del vettore */
    matrice = (char**)malloc(N * sizeof(char*));
    vettore = (char**)malloc(N * sizeof(char*));
    for (int i = 0; i < N; i++) {
        matrice[i] = (char*)malloc(N * sizeof(char));
        vettore[i] = (char*)malloc(N * sizeof(char));
    }

    /* Riempimento e stampa della matrice */
    printf("Matrice generata:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrice[i][j] = 'a' + rand() % 2;
            printf("%c ", matrice[i][j]);
        }
        printf("\n");
    }

    /* Creazione dei thread */
    for (int i = 0; i < NUMT; i++) {
        int *index = (int*)malloc(sizeof(int)); // Allocato solo un int
        *index = i;
        if (pthread_create(&th[i], NULL, ricerca, index) != 0) {
            perror("Errore nella creazione del thread");
            exit(1);
        }
    }

    if (pthread_create(&t_stampa, NULL, stampa, NULL) != 0) {
        perror("Errore nella creazione del thread stampa");
        exit(1);
    }

    /* Join dei thread */
    for (int i = 0; i < NUMT; i++) {
        pthread_join(th[i], NULL);
    }
    pthread_join(t_stampa, NULL);

    /* Deallocazione memoria */
    for (int i = 0; i < N; i++) {
        free(matrice[i]);
        free(vettore[i]);
    }
    free(matrice);
    free(vettore);

    sem_destroy(&sem);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
