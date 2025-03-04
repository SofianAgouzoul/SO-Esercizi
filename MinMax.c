/*CALCOLO DEL MASSIMO E DEL MINIMO DI UNA MATRICE DI INTERI
I THREAD PARI CALCOLANO IL MASSIMO, QUELLI DISPARI IL MINIMO
SPECIFICARE AI THREADS LE RIGHE PARI E QUELLE DISPARI NEL CONTROLLO DELL'IF*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 4
#define M 4
#define NUMT 4

int **matrice,*vettore, completed = 0;
pthread_mutex_t mutex;
pthread_mutex_t stampa;
pthread_cond_t cond;

void* ricercaMaxMin(void* arg){
    int j,index = *(int*)arg;
    int min = matrice[index][0] ,max = matrice[index][0];;

    for (int j = 0; j < M; j++) {
        if (index % 2 == 0) {  // Thread pari cercano il massimo
            if (matrice[index][j] > max) {
                max = matrice[index][j];
            }
        } else {  // Thread dispari cercano il minimo
            if (matrice[index][j] < min) {
                min = matrice[index][j];
            }
        }
    }

    /*AGGIORNAMENTO DI VETTORE CON I VALORI MASSIMI E MINIMI TROVATI*/
    pthread_mutex_lock(&mutex);
    vettore[index] = (index % 2 == 0) ? max : min;
    pthread_mutex_unlock(&mutex);

    /*INCREMENTO DELLA VARIABILE COMPLETED ED EVENTUALE SEGNALAZIONE AL THREAD DI STAMPA*/
    pthread_mutex_lock(&stampa);
    completed++;
    if(completed == NUMT){
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&stampa);

    /*LIBERO LA MEMORIA ALLOCATA ALL'INDICE*/
    free(arg);
    pthread_exit(NULL);
}

void* Stampa(){
    /*ATTESA DEL COMPLETAMENTO DEI THREADS NELLA FUNZIONE RICERCA*/
    pthread_mutex_lock(&stampa);
    while(completed < NUMT){
        pthread_cond_wait(&cond,&stampa);
    }
    pthread_mutex_unlock(&stampa);

    int sommaTotale = 0, prodTotale = 1;
    printf("VALORI DEL VETTORE\n");
    for(int i=0;i<N;i++){
        printf("%d ",vettore[i]);
        sommaTotale += vettore[i];
        prodTotale *= vettore[i];
    }
    printf("\nSOMMA TOTALE DEI VALORI TROVATI\n");
    printf("%d - %d\n",sommaTotale, prodTotale);

    pthread_exit(NULL);
}


int main(int argc, char** argv){
    srand(time(NULL));
    int i,j;
    pthread_t th[NUMT],threadStampa;

    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_init(&stampa,NULL);
    pthread_cond_init(&cond,NULL);

    matrice = (int**)calloc(N,sizeof(int*));
    for(i=0;i<N;i++){
        matrice[i] = (int*)calloc(M,sizeof(int));
    }

    vettore = (int*)calloc(N,sizeof(int));

    printf("MATRICE\n");
    for(i=0;i<N;i++){
        for(j=0;j<M;j++){
            matrice[i][j] = 1+rand()%100;
            printf("%d ",matrice[i][j]);
        }
        printf("\n");
    }

    printf("-------------------------\n");
    for(int i=0;i<NUMT;i++){
        int *index = (int*)malloc(sizeof(int));
        *index = i;
        if(pthread_create(&th[i],NULL,&ricercaMaxMin,(void*)index)!=0){
            perror("Errore nella create\n");
            exit(0);
        }
    }

    for(int i=0;i<NUMT;i++){
        if(pthread_join(th[i],NULL)!=0){
            perror("Errore nella join\n");
            exit(0);
        }
    }

    if(pthread_create(&threadStampa,NULL,&Stampa,NULL)!=0){
        perror("Errore create del thread stampa\n");
        exit(0);
    }

    if(pthread_join(threadStampa,NULL)!=0){
        perror("Errore join del thread stampa\n");
        exit(0);
    }

    // Deallocazione memoria
    for (int i = 0; i < N; i++) {
        free(matrice[i]);
    }
    free(matrice);
    free(vettore);

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&stampa);
    pthread_cond_destroy(&cond);

    
    return 0;
}