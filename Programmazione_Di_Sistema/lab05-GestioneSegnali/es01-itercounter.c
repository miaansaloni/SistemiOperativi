#include <stdio.h>
#include <signal.h> //sig_atomic_t
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_PROC_NUM 20

static volatile sig_atomic_t contatore = 0;

void gestore(int signo)
{
    printf("Il processo %d ha eseguito %d iterazioni per il segnale %d\n", getpid(), contatore, signo);
    exit(0);
}

int main(int argc, char **argv)
{
    int *pid;
    int Nf, Nsec, i, status;

    // controllo argomenti
    if (argc != 3)
    {
        fprint(stderr, "Uso: ./es01-itercounter <num_processi(Nf)> <num_secondi(Nsec)>\n");
        exit(1);
    }

    Nf = atoi(argv[1]);
    if (Nf <= 0)
    {
        fprintf(stderr, "Il parametro <num_processi(Nf)> deve essere un intero positivo");
        exit(2);
    }

    Nsec = atoi(argv[2]);
    if (Nsec <= 0)
    {
        fprintf(stderr, "Il parametro <num_secondi(Nsec)> deve essere un intero positivo");
        exit(4);
    }

    pid = (int *)malloc(Nf * sizeof(int));

    for (i = 0; i < Nf; ++i)
    {
        pid[i] = fork();

        if (pid[i] < 0)
        {
            perror("Errore nella fork\n");
            exit(5);
        }

        else if (pid[i] == 0)
        {
            free(pid);

            struct sigaction sa;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sa.sa_handler = gestore;

            if (sigaction(SIGUSR1, &sa, NULL) < 0)
            {
                perror("Errore in sigaction, SIGUSR1 figlio\n");
                exit(-3);
            }

            while (1)
            {
                sleep(1);
                contatore++;
            }
        }
    }
    sleep(Nsec);

    for (i = 0; i < Nf; ++i)
    {
        kill(pid[i], SIGUSR1);
    }

    free(pid);

    for (i = 0; i < Nf; i++)
    {
        wait(&status);
    }

    return 0;
}