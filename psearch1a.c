#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h> // gettimeofday için gerekli kütüphaneyi ekle

#define MAX_LINE_LENGTH 1024

// ... (diğer tanımlamalar ve

#define MAX_LINE_LENGTH 1024

// Structure to store a matched line information
typedef struct {
    char* filename;
    int line_number;
    char* line;
} Match;
// Function to perform the search and write results to a temporary file
void searchAndWrite(const char* keyword, const char* input_file, int child_index) {
    printf("Child process %d: Searching in %s\n", child_index, input_file);

    char temp_output_file[100];
    sprintf(temp_output_file, "temp_out_%d.txt", child_index);

    FILE* input = fopen(input_file, "r");
    if (input == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    FILE* output = fopen(temp_output_file, "w");
    if (output == NULL) {
        perror("Error opening temporary output file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 0;

    // Iterate through each line in the file
    while (fgets(line, sizeof(line), input) != NULL) {
        line_number++;
        if (strstr(line, keyword) != NULL) {
            fprintf(output, "%s, %d: %s", input_file, line_number, line);

        }
    }
    fclose(input);
    fclose(output);
        printf("Child process %d: Finished searching\n", child_index);

}

int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <keyword> <num_files> <input_file1> <input_file2> ... <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* keyword = argv[1];
    int num_files = atoi(argv[2]);

    // Check if the correct number of file names is provided
    if (argc != num_files + 4) {
        fprintf(stderr, "Invalid number of input files\n");
        exit(EXIT_FAILURE);
    }

    char** input_files = &argv[3];
    const char* output_file = argv[num_files + 3];
    struct timeval start, end;

    gettimeofday(&start, NULL); // Zaman ölçümüne başla

    // Create n children processes
    for (int i = 0; i < num_files; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Error creating child process");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            searchAndWrite(keyword, input_files[i], i);
            exit(EXIT_SUCCESS);
        }
        
    }

    // Wait for all children to finish
    int status;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0);
    gettimeofday(&end, NULL); // Zaman ölçümünü bitir

    long seconds = (end.tv_sec - start.tv_sec); // saniye cinsinden geçen süre
    long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec); // mikrosaniye cinsinden geçen süre

    // Merge temporary output files into the final output file
    FILE* finalOutFile = fopen(output_file, "w");
    if (finalOutFile == NULL) {
        perror("Error opening final output file");
        exit(EXIT_FAILURE);
    }

    // Iterate through each temporary output file
    for (int i = 0; i < num_files; i++) {
        char temp_output_file[100];
        sprintf(temp_output_file, "temp_out_%d.txt", i);

        FILE* tempOutFile = fopen(temp_output_file, "r");
        if (tempOutFile == NULL) {
            perror("Error opening temporary output file");
            exit(EXIT_FAILURE);
        }

        char line[MAX_LINE_LENGTH];

        // Copy the contents of the temporary output file to the final output file
        while (fgets(line, MAX_LINE_LENGTH, tempOutFile) != NULL) {
            fprintf(finalOutFile, "%s", line);
        }
        fclose(tempOutFile);
        remove(temp_output_file);
    }
    fclose(finalOutFile);


    printf("Search completed. Results written to %s\n", output_file);
    printf("Elapsed time: %ld seconds and %ld microseconds\n", seconds, micros);
    fflush(stdout); // stdout tamponunu boşalt


    return EXIT_SUCCESS;
}
