/*0 legge, 1 scrive
dup2 - figli -stdout
dup2 - padre - stdin*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int pipe1[2], pipe2[2]; // Due pipe: cat → grep, grep → wc
    pid_t pid1, pid2;

    // Creazione delle pipe
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Primo fork: esegue `cat /etc/passwd`
    if ((pid1 = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) { 
        // Processo figlio 1 → esegue `cat /etc/passwd`
        close(pipe1[0]);       // Chiude lettura pipe1
        dup2(pipe1[1], STDOUT_FILENO); // Redirige stdout alla pipe1
        close(pipe1[1]);       // Chiude scrittura pipe1 dopo dup2
        execlp("cat", "cat", "/etc/passwd", NULL);
        perror("execlp cat");
        exit(EXIT_FAILURE);
    }

    // Secondo fork: esegue `grep /bin/bash`
    if ((pid2 = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // Processo figlio 2 → esegue `grep /bin/bash`
        close(pipe1[1]);       // Chiude scrittura pipe1
        dup2(pipe1[0], STDIN_FILENO); // Legge da pipe1
        close(pipe1[0]);       // Chiude lettura pipe1 dopo dup2

        close(pipe2[0]);       // Chiude lettura pipe2
        dup2(pipe2[1], STDOUT_FILENO); // Scrive in pipe2
        close(pipe2[1]);       // Chiude scrittura pipe2 dopo dup2
        execlp("grep", "grep", "/bin/bash", NULL);
        perror("execlp grep");
        exit(EXIT_FAILURE);
    }

    // Processo padre esegue `wc -l`
    close(pipe1[0]); close(pipe1[1]); // Chiude pipe1
    close(pipe2[1]); // Chiude scrittura pipe2
    dup2(pipe2[0], STDIN_FILENO); // Legge da pipe2
    close(pipe2[0]); // Chiude lettura pipe2 dopo dup2
    execlp("wc", "wc", "-l", NULL);
    
    // Se execlp fallisce:
    perror("execlp wc");
    exit(EXIT_FAILURE);
}
