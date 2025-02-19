#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/*NEL PRODOTTO VETTORIALE, I VETTORI SONO TRIDIMENSIONALI E SONO DEI DOUBLE. 
NON SI CALCOLARE IL PRODOTTO DI VETTORI CON SIZE DIVERSO*/

#define N 3 /*SIZE DEL VETTORE 1*/
#define NUMT 3 /*NUMERO DI THREADS*/

double *v1,*v2,*v3;
int completed = 0;

pthread_mutex_t stampa;
pthread_mutex_t calcolo;
pthread_cond_t cond;

void* prodotto(void* arg) {
    int index = *(int*)arg;

    // Calcolo della componente index del prodotto vettoriale
    if (index == 0) {
        v3[0] = v1[1] * v2[2] - v1[2] * v2[1];
    } else if (index == 1) {
        v3[1] = v1[2] * v2[0] - v1[0] * v2[2];
    } else if (index == 2) {
        v3[2] = v1[0] * v2[1] - v1[1] * v2[0];
    }

    // Aggiornamento del numero di thread completati
    pthread_mutex_lock(&stampa);
    completed++;
    if (completed == NUMT) {
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&stampa);

    free(arg);  // Libero subito la memoria allocata per l'indice
    pthread_exit(NULL);
}


int main(int argc, char** argv){
    //srand(time(NULL));
    pthread_t th[NUMT];
    pthread_mutex_init(&stampa,NULL);
    pthread_mutex_init(&calcolo,NULL);
    pthread_cond_init(&cond,NULL);

    /*ALLOCAZIONE DEI VETTORI*/
    v1 = (double*)calloc(N,sizeof(double));
    v2 = (double*)calloc(N,sizeof(double));
    v3 = (double*)calloc(N,sizeof(double));

    printf("V1\n");
    for(int i=0;i<N;i++){
        v1[i] = rand()%10;
        printf("%f ",v1[i]);
    }
    printf("\nV2\n");
    for(int i=0;i<N;i++){
        v2[i] = rand()%15;
        printf("%f ",v2[i]);
    }
    printf("\n");


    /*CREAZIONE DEI THREADS*/
    for(int i=0;i<NUMT;i++){
        int *index = (int*)malloc(sizeof(int));
        *index = i;
        if(pthread_create(&th[i],NULL,prodotto,(void*)index)!=0){
            perror("Errore nella creazione dei threads\n");
            exit(0);
        }
    }

    for(int i=0;i<NUMT;i++){
        if(pthread_join(th[i],NULL)!=0){
            perror("Errore nella join dei threads\n");
            exit(0);
        }
    }

    /*IL THREAD MAIN DOVRA STAMPARE IL RISULTATO FINALE QUANDO I THREADS FINIRANNO DI CALCOLARE IL PRODOTTO*/
    pthread_mutex_lock(&stampa);
    while(completed < NUMT){
        pthread_cond_wait(&cond,&stampa);
    }
    pthread_mutex_unlock(&stampa);

    /* STAMPA DEL RISULTATO */
    printf("\nProdotto vettoriale:\n");
    printf("(%f, %f, %f)\n", v3[0], v3[1], v3[2]);

    /* DEALLOCAZIONE E DISTRUZIONE DELLE STRUTTURE */
    free(v1);
    free(v2);
    free(v3);


    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&calcolo);
    pthread_mutex_destroy(&stampa);

    return 0;
}