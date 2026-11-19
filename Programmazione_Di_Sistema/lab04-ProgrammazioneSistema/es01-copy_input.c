#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  //open
#include <unistd.h> //fork
#include <string.h>

int main(int argc, char **argv)
{
    int fd;
    char buff[1024];

    // controllo argomenti
    if (argc != 2)
    {
        fprintf(stderr, "Uso: es01-copy_input <nomefile>\n");
        exit(1);
    }

    // creazione del file con system call OPEN, con relativi flag e permessi (0644 rw-r--r--)
    fd = open(argv[1], O_WRONLY | O_TRUNC | O_CREAT, 0644);

    if (fd < 0)
    {
        fprintf(stderr, "Errore creazione file %s\n", argv[1]);
        exit(2);
    }
}