#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

void run_cmd(char *cmd, int input_fd, int output_fd) {
    dup2(input_fd, STDIN_FILENO);  // Duplicate input_fd to stdin
    dup2(output_fd, STDOUT_FILENO); // Duplicate output_fd to stdout
    close(input_fd); // Close the original file descriptors
    close(output_fd);
    execlp("/bin/sh", "sh", "-c", cmd, NULL); // Execute the command
    perror("execlp"); // In case execlp fails
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int pipefd[2];
    pid_t pid;
    int fin, fout;

    if (argc != 5) {
        fprintf(stderr, "Usage: %s <input_file> <cmd1> <cmd2> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the input and output files
    fin = open(argv[1], O_RDONLY);
    if (fin < 0) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }
    
    fout = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fout < 0) {
        perror("Error opening output file");
        close(fin);
        return EXIT_FAILURE;
    }

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        close(fin);
        close(fout);
        return EXIT_FAILURE;
    }

    // Fork a process
    pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        close(fin);
        close(fout);
        close(pipefd[0]);
        close(pipefd[1]);
        return EXIT_FAILURE;
    }

    if (pid == 0) { // Child process
        close(pipefd[0]); // Close the read end in the child
        run_cmd(argv[2], fin, pipefd[1]);
    } else { // Parent process
        close(pipefd[1]); // Close the write end in the parent
        close(fin); // Parent doesn't need the input file
        wait(NULL); // Wait for the child to finish
        run_cmd(argv[3], pipefd[0], fout);
    }

    return EXIT_SUCCESS;
}
