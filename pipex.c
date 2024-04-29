#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
    int fd_input;
    int fd_output;
    char *cmd1; 
    char *cmd2;

    cmd1 = strdup(argv[2]);
    cmd2 = strdup(argv[3]);
    if (argc != 5)
    {
        write(1, "Error\n", 6);
        exit(1);
    }
    fd_input = open(argv[1], O_RDONLY);
    if (fd_input < 0) {
        write(1, "Cant open input file\n", 16);
        exit(1);
    }

    // Open the output file
    fd_output = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_output < 0) {
        write(1, "Cant open output file\n", 17);
        close(fd_input); // Make sure to close the input file descriptor if output file open fails
        exit(1);
    }

    printf("fd input %d\n", fd_input);
    printf("fd output %d\n", fd_output);
    printf("cmd 1 is gonna be %s", cmd1);
    printf("cmd 2 is gonna be %s", cmd2);


    free(cmd1);
    free(cmd2);
    close(fd_input);
    close(fd_output);
    return (0);
}