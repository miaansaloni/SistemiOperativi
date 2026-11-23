#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

static volatile sig_atomic_t counter = 0;
static volatile sig_atomic_t child_index;
static volatile sig_atomic_t nChildren;
static volatile sig_atomic_t flagTerminato = false;

void sigintchild(int sig)
{
    printf("child %d:terminazione con valore counter %d\n", child_index, counter);
    exit(0);
}

void sigusr1_handler(int sig)
{
    counter++;
    printf("Child %d: inremento contatore di 1...\n", child_index);
    printf("Nuovo valore contatore child %d: %d\n", child_index, counter);
}

void sigusr2_handler(int sig)
{
    counter--;
    printf("Child %d: decremento contatore di 1...\n", child_index);
    printf("Nuovo valore contatore child %d: %d\n", child_index, counter);
}

void sigintparenthandler(int sig)
{
    printf("Ricevuto CTRL+C --> Attendo sincronizzazione con i children...\n");

    int status;
    for (int i = 0; i < nChildren; i++)
    {
        wait(&status);
    }

    printf("Tutti i children terminati, chiudo il programma\n");
    flagTerminato = true;
}

int main(int argc, char **argv)
{

    struct sigaction sigint, sigusr1, sigusr2, sigintparent;
    int *children;

    if (argc != 2)
    {
        fprintf(stderr, "Uso: ./%s <nChildren>\n", argv[0]);
        exit(1);
    }

    if (atoi(argv[1]) <= 0)
    {
        fprintf(stderr, "Ci deve essere almeno un child da poter generare\n");
        exit(2);
    }

    nChildren = atoi(argv[1]);
    children = (int *)malloc(nChildren * sizeof(int));

    for (int i = 0; i < nChildren; i++)
    {
        int pid = fork();

        if (pid == 0)
        {
            free(children);
            child_index = i;

            sigemptyset(&sigint.sa_mask);
            sigint.sa_flags = 0;
            sigint.sa_handler = sigintchild;
            if (sigaction(SIGINT, &sigint, NULL) == -1)
            {
                perror("Errore nell'installazione del gestore di SIGINT");
                exit(-11);
            }

            sigemptyset(&sigusr1.sa_mask);
            sigusr1.sa_flags = 0;
            sigusr1.sa_handler = sigusr1_handler;
            if (sigaction(SIGUSR1, &sigusr1, NULL) == -1)
            {
                perror("Errore nell'installazione del gestore di SIGUSR1");
                exit(-11);
            }

            sigemptyset(&sigusr2.sa_mask);
            sigusr2.sa_flags = 0;
            sigusr2.sa_handler = sigusr2_handler;
            if (sigaction(SIGUSR2, &sigusr2, NULL) == -1)
            {
                perror("Errore nell'installazione del gestore di SIGUSR2");
                exit(-11);
            }

            while (1)
            {
                pause();
            }
        }
        else if (pid < 0)
        {
            printf("Errore nella fork del child %d\n", i);
        }

        children[i] = pid; // salvo i children in un vettore per poterli selezionare casualmente
    }

    // PARENT
    sigemptyset(&sigintparent.sa_mask);
    sigintparent.sa_flags = 0;
    sigintparent.sa_handler = sigintparenthandler;
    if (sigaction(SIGINT, &sigintparent, NULL) == -1)
    {
        perror("Errore nell'installazione del gestore di SIGINT");
        exit(-11);
    }

    while (!flagTerminato)
    {
        int randIndex = rand() % nChildren;
        int sig = ((rand() % 2) == 0 ? SIGUSR1 : SIGUSR2);
        kill(children[randIndex], sig);
        sleep(2);
    }

    free(children);

    return 0;
}