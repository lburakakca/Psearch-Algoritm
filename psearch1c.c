#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>

#define MAX_LINE_LENGTH 256

// Function to search for the keyword in a specific file and write results to the pipe
void searchInFile(const char *filename, const char *keyword, int pipe_fd) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 1;

    while (fgets(line, sizeof(line), file) != NULL) {

        // Search for the keyword in the line
        if (strstr(line, keyword) != NULL) {
            char matchedLine[MAX_LINE_LENGTH];
            snprintf(matchedLine, sizeof(matchedLine), "%s, %d: %s", filename, line_number, line);
            write(pipe_fd, matchedLine, strlen(matchedLine));
            printf("Match found in %s at line %d: %s", filename, line_number, line);  // Debug çıktısı
        }
        line_number++;
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    struct timeval start, end; // Zaman ölçümü için değişkenleri tanımla

    if (argc < 5) {
        fprintf(stderr, "Usage: %s <keyword> <num_files> <input_file1> <input_file2> ... <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    gettimeofday(&start, NULL);
    const char *keyword = argv[1];
    int num_files = atoi(argv[2]);

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_files; ++i) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Child process
            searchInFile(argv[i + 3], keyword, pipe_fd[1]);
            close(pipe_fd[1]);  
            exit(EXIT_SUCCESS);
        }
    }

    // Parent process
    close(pipe_fd[1]);  

    FILE *output_file = fopen(argv[argc - 1], "w");
    if (output_file == NULL) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LINE_LENGTH];
    ssize_t bytesRead;

    // Read from the pipe and write to the output file
    while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, bytesRead, output_file);
    }

    fflush(output_file);
    fclose(output_file);
    close(pipe_fd[0]);

    // Wait for all child processes to finish
    for (int i = 0; i < num_files; ++i) {
        wait(NULL);
    }
    gettimeofday(&end, NULL); // Zaman ölçümünü bitir

    long seconds = (end.tv_sec - start.tv_sec); // saniye cinsinden geçen süre
    long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec); // mikrosaniye cinsinden geçen süre

    printf("Search completed. Results written to %s\n", argv[argc - 1]);
    printf("Elapsed time: %ld seconds and %ld microseconds\n", seconds, micros);
    return 0;
}
