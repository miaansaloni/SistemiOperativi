#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define DIM 50

int main(int argc, char **argv)
{
    int num, fd, pid, status;
    char nomefile[DIM];

    if (argc != 2)
    {
        printf(stderr, "Uso: anteprima <num>\n");
        exit(1);
    }

    if ((num = atoi(argv[1])) <= 0)
    {
        printf("Errore: %s deve essere un intero positivo maggiore di 0\n", argv[1]);
        exit(2);
    }

    printf("Inserire il nome del file di cui si desidera visualizzare l'anteprima, 'fine' per uscire:\n");
    scanf("%s", nomefile);

    while (strcmp(nomefile, "fine") != 0)
    {

        if ((fd = open(nomefile, O_RDONLY)) < 0) // per controllare che il file esista, provo ad aprirlo
        {
            if (errno == ENOENT) // se la open fallisce, controllo il valore di errno
            {
                printf("Il file %s non esiste\n", nomefile);
            }
            else
            {
                perror("open"); // errore generico
                exit(3);
            }
        }
        else
        {
            close(fd); // il file esiste allora lo chiudo per evitare resource leak

            // creo un figlio per visualizzarne l'anteprima
            pid = fork();
            if (pid < 0)
            {
                perror("fork");
                exit(4);
            }
            else if (pid == 0)
            {
                printf("\nAnteprima del file %s:\n", nomefile);
                execlp("head", "head", "-n", argv[1], nomefile, (char *)0);
                perror("exec");
                exit(5);
            }
            wait(&status); // il padre aspetta che il figlio termini
        }
        printf("\nInserisci il nome del file di cui vuoi visualizzare l'antprima, 'fine' per uscire: \n");
        scanf("%s", nomefile);
    }
    return 0;
}