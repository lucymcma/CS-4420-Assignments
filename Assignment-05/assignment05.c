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
    // for rr
    int remaining;
    // 1 if process has been enqueued for the last time
    int done;
} Process;

#define SIZE 50
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
 * @brief insert an arriving process to the ready queue, maintaining sort by burst time. break ties with FCFS
 * @param new_process process to be added
 */
 void insert(Process new_process) {
    if (ready_count >= SIZE) {
        printf("Error: Ready queue is full\n");
        exit(1);
    }
    // Find the correct insertion index
    int i = ready_count - 1;
    
    // shift elements right
    while (i >= 0 && (ready_queue[i].burst > new_process.burst || (ready_queue[i].burst == new_process.burst && ready_queue[i].arrival > new_process.arrival))) {
        ready_queue[i + 1] = ready_queue[i];
        i--;
    }
    // insert 
    ready_queue[i + 1] = new_process;
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

 /**
 * @brief output report of process data
 * @param processes array of processes
 * @param num_processes number of processes in array
 */
void output(Process processes[], int num_processes) {
    float total_waiting = 0;
    // table header
    printf("\n%-5s %-15s %-12s %-10s %-14s %-12s\n", "PID", "Arrival Time", "Start Time", "End Time", "Running Time", "Waiting Time");
    printf("---------------------------------------------------------------------------\n");
    // print data for each process
    for (int i = 0; i < num_processes; i++) {
        printf("%-5d %-15d %-12d %-10d %-14d %-12d\n", processes[i].pid, processes[i].arrival, processes[i].start, processes[i].end, processes[i].running, processes[i].waiting);
        total_waiting += processes[i].waiting;
    }
    printf("---------------------------------------------------------------------------\n");
    printf("Average Waiting Time = %.2f\n", total_waiting / num_processes);
}

 /**
 * @brief output report of process data for rr only
 * @param processes_history array of what processes ran
 * @param num_processes_history number of processes in history array
 */
 void output_rr(Process processes_history[], int num_processes_history) {
    // table header
    printf("\n%-5s %-12s %-10s %-14s\n", "PID", "Start Time", "End Time", "Running Time");
    printf("--------------------------------------------\n");
    // print data for each process
    for (int i = 0; i < num_processes_history; i++) {
        printf("%-5d %-12d %-10d %-14d\n", processes_history[i].pid, processes_history[i].start, processes_history[i].end, processes_history[i].running);
    }
    printf("--------------------------------------------\n");
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

    // quantum for RR
    int quantum = 0;
    if (strcmp(alg, "RR") == 0) {
        if (argc < 4) {
            fprintf(stderr, "No quantum provided\n");
            return 1;
        }
        quantum = atoi(argv[3]);
        if (quantum == 0) {
            fprintf(stderr, "Invalid quantum\n");
            return 1;
        }
    }

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
        processes[i].running = processes[i].burst;
        processes[i].remaining = processes[i].burst;
    }
    
    // close file stream
    fclose(file);

    // print info gathered from file
    if (d) {
        printf("%-5s %-15s %-12s\n", "PID", "Arrival Time", "Burst Time");
        printf("---------------------------------\n");
        for (int i = 0; i < num_processes; i++) {
            printf("%-5d %-15d %-12d\n", processes[i].pid, processes[i].arrival, processes[i].burst);
        }
        printf("---------------------------------\n\n");
    }

    // for tracking rr
    Process processes_history[SIZE];
    int num_processes_history = 0;

    // simulate scheduling algorithm
    int time = 0;
    int completed = 0;
    while(completed < num_processes) {

        // check for arriving processes
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival <= time && !processes[i].done) {
                processes[i].done = 1;
                if (strcmp(alg, "SJF") == 0)
                    insert(processes[i]);
                else 
                    enqueue(processes[i]);
            }
        }

        if (ready_count == 0) {
            time++;
            continue;
        }

        Process current = dequeue();

        // print debug info
        if (d) {
            printf("Time = %d\n", time);
            printf("Running: \n");
            printf("%-5s %-15s %-12s\n", "PID", "Arrival Time", "Burst Time");
            printf("%-5d %-15d %-12d\n", current.pid, current.arrival, current.burst);
            printf("%d processes in ready queue\n\n", ready_count);
        }

        // nonpreemptive
        if (strcmp(alg, "FCFS") == 0 || strcmp(alg, "SJF") == 0) {
            // update process data
            processes[current.pid].start = time;
            processes[current.pid].end = time + current.burst;
            processes[current.pid].waiting = time - processes[current.pid].arrival;

            // run the process
            time += current.burst;
            completed++;
        }
        // preemptive
        else if (strcmp(alg, "RR") == 0) {
            // for logging
            Process temp;
            temp.pid = current.pid;
            temp.start = time;

            // first time seeing process
            if (current.remaining == current.burst)
                processes[current.pid].start = time;

            // case 1: process does not complete during quantum
            if (current.remaining > quantum) {
                temp.running = quantum;
                // run
                time += quantum;
                temp.end = time;
                current.remaining -= quantum;
                processes[current.pid].remaining = current.remaining;
                
                // check before enqueuing current
                for (int i = 0; i < num_processes; i++) {
                    if (processes[i].arrival <= time && !processes[i].done) {
                        enqueue(processes[i]);
                        processes[i].done = 1;
                    }
                }
                enqueue(processes[current.pid]);
            }
            // case 1: process completes during quantum
            else {
                temp.running = current.remaining;
                // run
                time += current.remaining;
                temp.end = time;
                processes[current.pid].remaining = 0;
                processes[current.pid].end = time;
                processes[current.pid].waiting = time - processes[current.pid].arrival - processes[current.pid].burst;
                completed++;

            }
            if (num_processes_history == SIZE) {
                fprintf(stderr, "Too many processes");
                return 1;
            }
            processes_history[num_processes_history] = temp;
            num_processes_history++;
        }
        // invalid scheduling algorithm
        else {
            fprintf(stderr, "Usage: %s input_file [FCFS|RR|SJF] [time_quantum]", argv[0]);
            return 1;
        }
    }    
    
    // sort processes by start time
    for (int i = 0; i < num_processes - 1; i++) {
        for (int j = 0; j < num_processes - i - 1; j++) {
            if (processes[j].start > processes[j + 1].start) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }

    if (strcmp(alg, "RR") == 0)
        output_rr(processes_history, num_processes_history);
    output(processes, num_processes);

    return 0;
}
