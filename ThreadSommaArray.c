#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#define NUMTHREAD 4
#define N 4


pthread_mutex_t mutex;
pthread_cond_t condFine;

int **matrice, *vettore, completed = 0;

void* routine(void* arg){
    int j,index = *(int*)arg;
    int sumP;

    //OGNI THREAD CALCOLA LA SOMMA DI UNA RIGA DELLA MATRICE
    //IL RISULTATO DI OGNI RIGA E' MEMORIZZATO IN UNA CELLA DELL'ARRAY (VETTORE)
    sumP = 0;
    for(j=0;j<N;j++){
        sumP += matrice[index][j];
    }

    pthread_mutex_lock(&mutex);
    vettore[index] = sumP;
    printf("Thread %d || Somma riga %d = %d\n", index, index, sumP);
    completed++;
    if(completed == NUMTHREAD){
        pthread_cond_signal(&condFine);
    }
    pthread_mutex_unlock(&mutex);

    free(arg);
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    pthread_t th[NUMTHREAD];
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&condFine,NULL);

    //ALLOCAZIONE VETTORE E MATRICE
    matrice = (int**)calloc(N,sizeof(int*));
    for (int i = 0; i < N; i++) {
        matrice[i] = (int*)calloc(N,sizeof(int));
    }

    vettore = (int*)calloc(N,sizeof(int));

    //INIZIALIZZAZIONE DELLA MATRICE E STAMPA VALORI
    printf("MATRICE\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrice[i][j] = rand() % 100;
            printf("%d ", matrice[i][j]);
        }
        printf("\n");
    }

    printf("--------------------------------------\n");

    //CREAZIONE E JOIN DEI THREADS
    for(int i=0;i<NUMTHREAD;i++){
        int *a = malloc(sizeof(int));
        *a = i;
        if(pthread_create(&th[i],NULL,&routine,a)!=0){
            perror("Create() error\n");
        }
    }

    for(int i=0;i<NUMTHREAD;i++){
        if(pthread_join(th[i],NULL)!=0){
            perror("Join() error\n");
        }
    }

    //SE TUTTI I THREADS HANNO FINITO DI CALCOLARE LE SOMME PARZIALI, IL MAIN THREAD SI SVEGLIA E CALCOLA LA SOMMA TOTALE
    pthread_mutex_lock(&mutex);
    while(completed < NUMTHREAD){
        pthread_cond_wait(&condFine,&mutex);
    }

    int sommaTotale = 0;
    for(int i=0;i<N;i++){
        sommaTotale += vettore[i];
    }
    printf("Somma totale = %d\n",sommaTotale);

    pthread_mutex_unlock(&mutex);

    // Deallocazione memoria
    for (int i = 0; i < N; i++) {
        free(matrice[i]);
    }
    free(matrice);
    free(vettore);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condFine);

    return 0;
}