#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define N 4 /*SIZE DEL VETTORE*/
#define NUMT 4 /*NUMERO DI THREADS*/

double *v1, *v2, risultato = 0;
int completed = 0;

pthread_mutex_t stampa;
pthread_mutex_t calcolo;
pthread_cond_t cond;

void* prodotto_scalare(void* arg) {
    int index = *(int*)arg;
    double parziale = v1[index] * v2[index];
    free(arg);

    pthread_mutex_lock(&calcolo);
    risultato += parziale;
    completed++;
    if (completed == NUMT) {
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&calcolo);

    return NULL;
}

int main(int argc, char** argv) {
    srand(time(NULL));
    pthread_t th[NUMT];
    pthread_mutex_init(&stampa, NULL);
    pthread_mutex_init(&calcolo, NULL);
    pthread_cond_init(&cond, NULL);

    /* ALLOCAZIONE DEI VETTORI */
    v1 = (double*)calloc(N, sizeof(double));
    v2 = (double*)calloc(N, sizeof(double));

    printf("V1\n");
    for (int i = 0; i < N; i++) {
        v1[i] = rand() % 10;
        printf("%f ", v1[i]);
    }
    printf("\nV2\n");
    for (int i = 0; i < N; i++) {
        v2[i] = rand() % 10;
        printf("%f ", v2[i]);
    }
    printf("\n");

    /* CREAZIONE DEI THREADS */
    for (int i = 0; i < NUMT; i++) {
        int *index = (int*)malloc(sizeof(int));
        *index = i;
        if (pthread_create(&th[i], NULL, prodotto_scalare, (void*)index) != 0) {
            perror("Errore nella creazione dei threads\n");
            exit(0);
        }
    }

    for (int i = 0; i < NUMT; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Errore nella join dei threads\n");
            exit(0);
        }
    }

    /* IL THREAD MAIN ASPETTA CHE TUTTI I THREADS ABBIANO FINITO */
    pthread_mutex_lock(&stampa);
    while (completed < NUMT) {
        pthread_cond_wait(&cond, &stampa);
    }
    pthread_mutex_unlock(&stampa);

    /* STAMPA DEL RISULTATO */
    printf("\nProdotto scalare: %f\n", risultato);

    /* DEALLOCAZIONE E DISTRUZIONE DELLE STRUTTURE */
    free(v1);
    free(v2);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&calcolo);
    pthread_mutex_destroy(&stampa);

    return 0;
}
