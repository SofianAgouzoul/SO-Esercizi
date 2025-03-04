/*CALCOLO DELLA RADICE DI OGNI RIGA DI UN MATRICE*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define T 4 //NUMERO DI THREADS
#define N 4 //SIZE DELLA MATRICE

int completed = 0;
double **matrice,*vettore;

pthread_mutex_t mutexStampa;
pthread_mutex_t mutexCalcolo;
pthread_cond_t cond;

void* routine(void* arg){
    int j,index = *(int*)arg;
    double somma = 0;
    
    for(j=0;j<N;j++){
        somma += matrice[index][j];
    }

    /*AGGIORNAMENTO DEL VETTORE DELLE RADICI*/
    pthread_mutex_lock(&mutexCalcolo);
    vettore[index] = sqrt(somma);
    pthread_mutex_unlock(&mutexCalcolo);

    /*AGGIORNAMENTO DEL COUNTER DEI THREADS COMPLETATI*/
    pthread_mutex_lock(&mutexStampa);
    completed++;
    if(completed == T){
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutexStampa);

    free(arg);
    pthread_exit(NULL);
}

void* stampa(){
    pthread_mutex_lock(&mutexStampa);
    while(completed < T){
        pthread_cond_wait(&cond,&mutexStampa);
    }
    pthread_mutex_unlock(&mutexStampa);

    /*Stampa del risultato finale*/
    printf("Radici di ogni riga\n");
    for(int i=0;i<N;i++){
        printf("%f ",vettore[i]);
    }
    printf("\n");
    pthread_exit(NULL);
}

int main(int argc, char** argv){
    srand(time(NULL));
    int i,j;
    pthread_t th[T], t_stampa;
    pthread_mutex_init(&mutexCalcolo,NULL);
    pthread_mutex_init(&mutexStampa,NULL);
    pthread_cond_init(&cond,NULL);

    /*ALLOCAZIONE DELLA MATRICE E DEL VETTORE*/
    matrice = (double**)calloc(N,sizeof(double*));
    for(i=0;i<N;i++){
        matrice[i] = (double*)calloc(N,sizeof(double));
    }

    vettore = (double*)calloc(N,sizeof(double));


    /*INIZIALIZZAZIONE DELLA MATRICE*/
    printf("MATRICE\n");
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            matrice[i][j] = 1+rand()%100;
            printf("%f ",matrice[i][j]);
        }
        printf("\n");
    }

    /*CREAZIONE DEI THREADS*/
    for(i=0;i<T;i++){
        int *index = (int*)malloc(sizeof(int));
        *index = i;
        if(pthread_create(&th[i],NULL,&routine,(void*)index)!=0){
            perror("Create error()\n");
            exit(EXIT_FAILURE);
        }
    }

    for(i=0;i<T;i++){
        if(pthread_join(th[i],NULL)!=0){
            perror("Join error()\n");
            exit(EXIT_FAILURE);
        }
    }

    if(pthread_create(&t_stampa,NULL,&stampa,NULL)!=0){
        perror("Create2 error()\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_join(t_stampa,NULL)!=0){
        perror("Join 2 error()\n");
        exit(EXIT_FAILURE);
    }

    
    /*LIBERO LA MEMORIA ALLOCATA*/
    for(i=0;i<N;i++){
        free(matrice[i]);
    }
    free(matrice);
    free(vettore);
    

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutexCalcolo);
    pthread_mutex_destroy(&mutexStampa);

    return 0;
}