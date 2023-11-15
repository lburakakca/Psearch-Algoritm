#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <sys/time.h>

#define MAX_LINE_LENGTH 1024

// Shared data structure for storing results
struct Result {
    char filename[MAX_LINE_LENGTH];
    int line_number;
    char line[MAX_LINE_LENGTH];
};

int main(int argc, char *argv[]) {
    struct timeval start, end; // Zaman ölçümü için değişkenleri tanımla

    if (argc < 5) {
        fprintf(stderr, "Usage: %s <keyword> <num_files> <input_file1> <input_file2> ... <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *keyword = argv[1];
    int num_files = atoi(argv[2]);
    char **input_files = argv + 3;
    const char *output_file = argv[argc - 1];
    gettimeofday(&start, NULL);

    // Create shared memory for results
    struct Result *shared_results = mmap(NULL, sizeof(struct Result) * num_files, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // Initialize semaphore
    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_open("/shared_data", 1, 1);
    FILE *output = fopen(output_file, "w");
    for (int i = 0; i < num_files; ++i) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            FILE *file = fopen(input_files[i], "r");
            if (!file) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            char line[MAX_LINE_LENGTH];
            int line_number = 0;

            while (fgets(line, sizeof(line), file) != NULL) {
                ++line_number;

                // Check if the keyword is present in the line
                if (strstr(line, keyword) != NULL) {
                    sem_wait(sem);

                    // Store result in shared memory
                    snprintf(shared_results[i].filename, sizeof(shared_results[i].filename), "%s", input_files[i]);
                    shared_results[i].line_number = line_number;
                    snprintf(shared_results[i].line, sizeof(shared_results[i].line), "%s", line);
                    fprintf(output, "%s, %d: %s", shared_results[i].filename, shared_results[i].line_number, shared_results[i].line);


                    sem_post(sem);
                }
            }

            fclose(file);
            exit(EXIT_SUCCESS);
        }
    }

    // Wait for all child processes to complete
    for (int i = 0; i < num_files; ++i) {
        wait(NULL);
    }
    gettimeofday(&end, NULL); // Zaman ölçümünü bitir

    long seconds = (end.tv_sec - start.tv_sec); // saniye cinsinden geçen süre
    long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec); // mikrosaniye cinsinden geçen süre

    fclose(output);
    sem_close(sem);
    // Clean up
    munmap(sem, sizeof(sem_t));
    munmap(shared_results, sizeof(struct Result) * num_files);
    printf("Elapsed time: %ld seconds and %ld microseconds\n", seconds, micros);

    return EXIT_SUCCESS;
}
