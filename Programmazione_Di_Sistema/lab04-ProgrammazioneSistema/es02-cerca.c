#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    int pid, fd, status, i;

    if (argc < 3)
    {
        fprintf(stderr, "Uso: es02-cerca <nomefile> <stringa1> ... <stringaN>\n");
        exit(1);
    }

    fd = open("conteggio.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);

    if (fd < 0)
    {
        perror("Errore nella creazione del file di conteggio, termino\n");
        exit(2);
    }
    close(fd);

    for (i = 2; i < argc; i++)
    {
        pid = fork();

        if (pid < 0)
        {
            perror("fork");
            exit(3);
        }
        else if (pid == 0)
        {
            /*FIGLIO*/

            char log[256];
            fd = open("conteggio.txt", O_WRONLY | O_APPEND); // apertura del file in append
            sprintf(log, "%s %s\n", argv[1], argv[i]);       // creazione del log dell'operazione di conteggio
            write(fd, log, strlen(log));                     // scrittura del log sul file
            close(fd);

            printf("\n Numero di righe in cui compare la stringa %s:\n", argv[i]);
            execlp("grep", "grep", "-c", argv[i], argv[1], (char *)0); // chiamata a grep -c stringa nomefile, funziona senza ritorno
            perror("exec");                                            // errore nell'esecuzione di execlp
            exit(4);
        }
        // se sono qui sono il padre
    }
    // se sono qui sono il PADRE e ho finito di avviare processi figli
    for (i = 2; i < argc; i++)
    {
        /* Attendo la terminazione di uno dei figli */
        wait(&status);
    }

    return 0;
}