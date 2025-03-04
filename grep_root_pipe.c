#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char** argv){
    pid_t pid1,pid2;
    int pipe1[2],pipe2[2];

    if(pipe(pipe1)==-1 || pipe(pipe2)==-1){
        perror("Pipe\n");
        exit(EXIT_FAILURE);
    }

    /*Primo fork() esegue ls -al*/
    if((pid1 = fork())==-1){
        perror("Fork1\n");
        exit(EXIT_FAILURE);
    }

    /*Processo figlio 1 esegue ls -al*/
    if(pid1 == 0){
        close(pipe1[0]);
        dup2(pipe1[1],STDOUT_FILENO);
        close(pipe1[1]);
        execlp("ls","ls","-al",NULL);
        perror("Ls error\n");
        exit(EXIT_FAILURE);
    }
    
    /*Processo figlio pid2*/
    if((pid2 = fork())==-1){
        perror("Fork2\n");
        exit(EXIT_FAILURE);
    }

    /*Processo figlio 2 esegue grep root */
    if(pid2==0){
        close(pipe1[1]);
        dup2(pipe1[0],STDIN_FILENO);
        close(pipe1[0]);

        close(pipe2[0]);
        dup2(pipe2[1],STDOUT_FILENO);
        execlp("grep","grep","root",NULL);
        close(pipe2[1]);
        perror("Exec pid2\n");
        exit(EXIT_FAILURE);
    }

    /*Processo padre esegue wc -l*/
    close(pipe1[0]);
    close(pipe1[1]);
    dup2(pipe2[0],STDIN_FILENO);
    close(pipe2[0]);
    execlp("wc","wc","-l",NULL);

    /*Se fallisce*/
    perror("Exec padre\n");
    exit(EXIT_FAILURE);
}