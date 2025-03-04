/*ESEMPIO DI CREAZIONE DI UNA PIPE*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char** argv){
    int fd[2]; //0: per leggere --- // 1: per scrivere --- // 2: errore
    pid_t pid;
    char buffer[100];

    if(pipe(fd)==-1){
        perror("Pipe\n");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if(pid < 0){
        perror("Fork");
        exit(EXIT_FAILURE);
    }

    if(pid > 0){
        close(fd[0]); //chiude il padre in lettura
        char msg[] = "Ciao dal padre";
        write(fd[1],msg,strlen(msg)+1); //il padre scrive il messaggio nel buffer
        close(fd[1]); //chiude il file descriptor una volta scritto nel buffer
    }
    else{
        close(fd[1]);
        read(fd[0],buffer,sizeof(buffer));
        printf("Figlio ricevuto: %s\n",buffer);
        close(fd[0]);
    }

    return 0;
}