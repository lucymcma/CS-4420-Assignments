/**
 * @file assignment05.c
 * @author Lucy McManamon (lm845822@ohio.edu)
 * @brief simulates FCFS, RR, and SJF scheduling for given tasks and computes the average waiting time.
 * @date 10-02-2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// holds process data
typedef struct {
    int pid;
    int arrival;
    int burst;
    int start; 
    int end;
    int running;
    int waiting;
    // 1 if process has been ran
    int done;
} Process;

// max number of processes, max number allowed in ready queue
const int SIZE = 50;
Process ready_queue[SIZE];
int ready_count = 0;

// debug flag
int d = 0;

/**
 * @brief adds an arriving process to the back of the ready queue
 * @param new_process process to be added
 */
 void enqueue(Process new_process) {
    if (ready_count >= SIZE) {
        printf("Error: Ready queue is full\n");
        exit(1);
    }
    ready_queue[ready_count] = new_process;
    ready_count++;
 } 

/**
 * @brief removes the process at the front of the ready queue, shifts remaining elements left
 * @return the process at the front of the ready queue
 */
 Process dequeue() {
    Process front = ready_queue[0];
    // shift all remaining elements left
    for (int i = 1; i < ready_count; i++) {
        ready_queue[i - 1] = ready_queue[i];
    }
    ready_count--;
    return front;
 }

 void output(Process processes[], int num_processes) {
    float total_waiting = 0;
    // table header
    printf("%-5s %-15s %-12s %-10s %-14s %-12s\n", "PID", "Arrival Time", "Start Time", "End Time", "Running Time", "Waiting Time");
    printf("---------------------------------------------------------------------------\n");
    // print data for each process
    for (int i = 0; i < num_processes; i++) {
        printf("%-5d %-15d %-12d %-10d %-14d %-12d\n", processes[i].pid, processes[i].arrival, processes[i].start, processes[i].end, processes[i].running, processes[i].waiting);
        total_waiting += processes[i].waiting;
    }
    printf("---------------------------------------------------------------------------\n");
    printf("Average Waiting Time = %.2f\n", total_waiting / num_processes);

 }

int main(int argc, char *argv[]) {
    // validate command line arguments
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input_file [FCFS|RR|SJF] [time_quantum]", argv[0]);
        return 1;
    }

    // open input file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening input file");
        return 1;
    }

    char *alg = argv[2];

    // read number of processes from input file
    int num_processes;
    if (fscanf(file, "%d", &num_processes) != 1) {
        fprintf(stderr, "Error: Invalid file format.\n");  
        return 1;
    }

    // read process table into array
    Process processes[SIZE];
    for (int i = 0; i < num_processes; i++) {
        fscanf(file, "%d %d %d", &processes[i].pid, &processes[i].arrival, &processes[i].burst);
        processes[i].done = 0;
    }
    
    // close file stream
    fclose(file);

    // print info gathered from file
    if (d) {
        printf("%-5s %-15s %-12s\n", "PID", "Arrival Time", "Burst Time");
        printf("----------------------------------------\n");
        for (int i = 0; i < num_processes; i++) {
            printf("%-5d %-15d %-12d\n", processes[i].pid, processes[i].arrival, processes[i].burst);
        }
        printf("----------------------------------------\n");
    }

    // simulate scheduling algorithm
    int time = 0;
    int completed = 0;
    while(completed < num_processes) {
        // add arriving processes to ready queue
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival <= time && !processes[i].done) {
                enqueue(processes[i]);
                processes[i].done = 1;
            }
        }
        if (strcmp(alg, "FCFS") == 0) {
            Process current = dequeue();

            // print debug info
            if (d) {
                printf("Time = %d\n", time);
                printf("Running: \n");
                printf("%-5s %-15s %-12s\n", "PID", "Arrival Time", "Burst Time");
                printf("%-5d %-15d %-12d\n", current.pid, current.arrival, current.burst);
                printf("%d processes in ready queue\n", ready_count);
                printf("----------------------------------------\n");
            }

            // update process data
            processes[current.pid].start = time;
            processes[current.pid].end = time + current.burst;
            processes[current.pid].running = current.burst; 
            processes[current.pid].waiting = time - processes[current.pid].arrival;

            // run the process
            time += current.burst;
            completed++;
        }
        // invalid scheduling algorithm
        else {
            fprintf(stderr, "Usage: %s input_file [FCFS|RR|SJF] [time_quantum]", argv[0]);
            return 1;
        }
    }

    output(processes, num_processes);

    return 0;
}
