/*SCRIVERE UN PROGRAMMA CHE IMPLEMENTI LA SEGUENTE PIPE DI COMANDI: ls -al | wc -l*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char** argv){
    int fd[2];
    pid_t pid;

    if(pipe(fd) == -1){
        perror("Pipe\n");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if(pid == -1){
        perror("Fork\n");
        exit(EXIT_FAILURE);
    }

    if(pid==0){//Processo figlio esegue la -al
        close(fd[0]);
        dup2(fd[1],STDOUT_FILENO);
        close(fd[1]);
        execlp("ls","ls","-al",NULL);
        perror("Errore execlp figlio\n");
        exit(EXIT_FAILURE);
    }
    else{//Processo padre esegue wc -l
        close(fd[1]);
        dup2(fd[0],STDIN_FILENO);
        close(fd[0]);
        execlp("wc","wc","-l",NULL);
        perror("Errore exec padre\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}