/*CREAZIONE DI UNA PIPE CHE ESEGUE IL COMANDO ls | wc -l*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char** argv){
    int fd[2];
    pid_t pid;
    
    if(pipe(fd)==-1){
        perror("Pipe\n");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if(pid<0){
        perror("Fork\n");
        exit(EXIT_FAILURE);
    }

    if(pid == 0){//PROCESSO FIGLIO
        close(fd[0]);
        dup2(fd[1],STDOUT_FILENO);
        close(fd[1]);
        execlp("ls","ls",NULL);
        perror("Exec\n");
        exit(EXIT_FAILURE);
    }
    else{//PROCESSO PADRE
        close(fd[1]);
        dup2(fd[0],STDIN_FILENO);
        close(fd[0]);
        execlp("wc","wc","-l",NULL);
        perror("Exec padre\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}