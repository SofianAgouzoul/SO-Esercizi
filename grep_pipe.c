/*GREP BASH PIPE*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc,char** argv){
    int fd[2]; //0: leggere --- //1: scrivere
    pid_t pid;

    if(pipe(fd)==-1){
        perror("Pipe\n");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if(pid < 0){
        perror("Pipe\n");
        exit(EXIT_FAILURE);
    }

    if(pid==0){//figlio
        close(fd[0]);
        dup2(fd[1],STDOUT_FILENO);
        close(fd[1]);
        execlp("ps","ps","aux",NULL); 
        //ps aux visualizza tutti i processi in esecuzione nel sistema
        //a:processi di tutti gli utenti --- 
        //u:informazioni dettagliate sui processi  --- 
        //x: processi non collegati al terminale
        perror("exec figlio\n");
        exit(EXIT_FAILURE);
    }
    else{//padre
        close(fd[1]);
        dup2(fd[0],STDIN_FILENO);
        close(fd[0]);
        execlp("grep","grep","bash",NULL);
        //grep bash: filtra e mostra le righe che contengono la parola "bash"
        perror("Exec padre\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}