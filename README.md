# Introduction
This report encompasses the usage and performance analysis of three distinct programs within the "psearch" series—psearch1a, psearch1b, and psearch1c. These programs are designed to search for a specified keyword across multiple input files and organize the findings by file name and line number.

# Overview of the Programs
- **psearch1a**: Utilizes child processes to individually process each input file. The results are stored in temporary files and later consolidated by the main process into a final output file.
- **psearch1b**: Employs shared memory (mmap) and unnamed semaphores for inter-process synchronization.
- **psearch1c**: Implements an anonymous (unnamed) pipeline (pipe) for efficient data transmission between processes.

# Installation and Execution Instructions
All three programs are compiled using the GCC (GNU Compiler Collection), necessary for running them on Linux or Unix-like operating systems. The compilation and execution process is as follows:

## Compilation:
- Each program is compiled using the gcc command in the command line.
- Example: `gcc psearch1a.c -o psearch1a` compiles the psearch1a program and creates an executable file named `psearch1a`.

## Execution:
- The compiled program is executed from the command line with specific parameters.
- These parameters typically include the keyword, the number of input files, and the names of the files.
- Example: `./psearch1a beef file1.txt file2.txt file3.txt output.txt` searches for the keyword `beef` in `file1.txt`, `file2.txt`, and `file3.txt`, writing the results to `output.txt`.

# Results and Comparative Analysis
Each program has its unique advantages and disadvantages, varying based on the parallel processing techniques and synchronization mechanisms used. This analysis is crucial in understanding how each program performs in specific scenarios and which scenarios they are best suited for.

# Limitations and Challenges
Each program has its limitations and challenges affecting its use and performance. For instance, psearch1a might experience a performance decline when processing a large number of files, psearch1b’s shared memory and semaphore usage could become complex in more intricate scenarios, and psearch1c may be limited by pipeline capacity and data transmission speed.
