#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUMTHREAD 4
#define N 4
#define M 4

sem_t sem;

int **A,**B,**C;
int completed = 0;

void* sommaMatrice(void* arg){
    int i = *(int*)arg;
    int j;

    sem_wait(&sem);
    for(j=0;j<N;j++){
        C[i][j] = A[i][j] + B[i][j];
        printf("%d ",C[i][j]);
    }
    printf("\n");
    sem_post(&sem);

    free(arg);
}

int main(int argc, char** argv){
    srand(time(NULL));
    int i,j;
    pthread_t th[NUMTHREAD];
    
    sem_init(&sem,0,1);

    /*ALLOCAZIONE DELLE MATRICI*/
    A = (int**)calloc(N,sizeof(int*));
    for(i=0;i<M;i++){
        A[i] = (int*)calloc(M,sizeof(int));
    }

    B = (int**)calloc(N,sizeof(int*));
    for(i=0;i<M;i++){
        B[i] = (int*)calloc(M,sizeof(int));
    }

    C = (int**)calloc(N,sizeof(int*));
    for(i=0;i<M;i++){
        C[i] = (int*)calloc(M,sizeof(int));
    }

    /*INIZIALIZZAZIONE DELLE MATRICI A E B*/
    printf("MATRICE A\n");
    for(i=0;i<N;i++){
        for(j=0;j<M;j++){
            A[i][j] = 1+rand()%10;
            printf("%d ",A[i][j]);
        }
        printf("\n");
    }

    printf("MATRICE B\n");
    for(i=0;i<N;i++){
        for(j=0;j<M;j++){
            B[i][j] = 1+rand()%10;
            printf("%d ",B[i][j]);
        }
        printf("\n");
    }

    printf("--------------------------------------\n");

    /*CREAZIONE ED INIZIALIZZAZIONE DEI THREADS*/
    for(int i=0;i<NUMTHREAD;i++){
        int *a = malloc(sizeof(int));
        *a = i;
        if(pthread_create(&th[i],NULL,&sommaMatrice,a)!=0){
            perror("Create() error\n");
        }
    }

    for(int i=0;i<NUMTHREAD;i++){
        if(pthread_join(th[i],NULL)!=0){
            perror("Join() error\n");
        }
    }


    sem_destroy(&sem);
    return 0;
}