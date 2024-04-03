#include <stdio.h>
#include <stdlib.h>

// Define PCB structure
struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int remainingTime; // Remaining time to finish execution
    int startTime;
    int finishTime;
    int waitingTime;
    int turnaroundTime;
    int responseTime;
    int numContextSwitch;
};

// Define Queue Node structure
struct QueueNode {
    struct Process process;
    struct QueueNode *next;
};

// Define Queue structure
struct Queue {
    struct QueueNode *front, *rear;
};

// Function to create a new Queue Node
struct QueueNode* createQueueNode(struct Process process) {
    struct QueueNode* newNode = (struct QueueNode*)malloc(sizeof(struct QueueNode));
    newNode->process = process;
    newNode->next = NULL;
    return newNode;
}

// Function to initialize a Queue
struct Queue* createQueue() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->front = queue->rear = NULL;
    return queue;
}

// Function to enqueue a process into the Queue based on remaining burst time
void enqueueSRTF(struct Queue* queue, struct Process process) {
    struct QueueNode* newNode = createQueueNode(process);

    if (queue->front == NULL || process.remainingTime < queue->front->process.remainingTime) {
        newNode->next = queue->front;
        queue->front = newNode;
    } else {
        struct QueueNode* temp = queue->front;
        while (temp->next != NULL && temp->next->process.remainingTime <= process.remainingTime) {
            temp = temp->next;
        }
        newNode->next = temp->next;
        temp->next = newNode;
    }

    if (newNode->next == NULL) {
        queue->rear = newNode;
    }
}

// Function to dequeue a process from the Queue
struct Process dequeue(struct Queue* queue) {
    if (queue->front == NULL) {
        struct Process emptyProcess;
        emptyProcess.pid = -1; // Return an empty process if the queue is empty
        return emptyProcess;
    }

    struct QueueNode* temp = queue->front;
    struct Process process = temp->process;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    return process;
}

// Function to check if the Queue is empty
int isEmpty(struct Queue* queue) {
    return queue->front == NULL;
}

// FCFS scheduling algorithm
void FCFS(struct Process processes[], int n) {
    struct Queue* readyQueue = createQueue();
    int currentTime = 0;
    float totalWaitingTime = 0, totalTurnaroundTime = 0, totalResponseTime = 0;
    int totalContextSwitch = 0;

    int completedProcesses = 0;

    // Enqueue processes based on their arrival time
    for (int i = 0; i < n; i++) {
        while (processes[i].arrivalTime > currentTime) {
            currentTime++;
        }
        processes[i].startTime = currentTime;
        enqueue(readyQueue, processes[i]);
    }

    // Process each task in the ready queue
    while (!isEmpty(readyQueue)) {
        struct Process currentProcess = dequeue(readyQueue);
        if (currentProcess.pid != -1) { // A valid process is dequeued
            currentTime += currentProcess.burstTime;
            currentProcess.finishTime = currentTime;
            currentProcess.turnaroundTime = currentProcess.finishTime - currentProcess.arrivalTime;
            currentProcess.waitingTime = currentProcess.turnaroundTime - currentProcess.burstTime;
            currentProcess.responseTime = currentProcess.startTime - currentProcess.arrivalTime;
            totalWaitingTime += currentProcess.waitingTime;
            totalTurnaroundTime += currentProcess.turnaroundTime;
            totalResponseTime += currentProcess.responseTime;

            printf("PID %d: Arrival Time: %d, Burst Time: %d, Start Time: %d, Finish Time: %d\n",
                   currentProcess.pid, currentProcess.arrivalTime, currentProcess.burstTime,
                   currentProcess.startTime, currentProcess.finishTime);
        }
    }

    totalWaitingTime /= n;
    totalTurnaroundTime /= n;
    totalResponseTime /= n;

    printf("Average Waiting Time: %.2f\n", totalWaitingTime);
    printf("Average Turnaround Time: %.2f\n", totalTurnaroundTime);
    printf("Average Response Time: %.2f\n", totalResponseTime);
}

// SRTF scheduling algorithm
void SRTF(struct Process processes[], int n) {
    struct Queue* readyQueue = createQueue();
    int currentTime = 0;
    float totalWaitingTime = 0, totalTurnaroundTime = 0, totalResponseTime = 0;
    int totalContextSwitch = 0;

    int completedProcesses = 0;

    // Initially, all processes have remaining time equal to burst time
    for (int i = 0; i < n; i++) {
        processes[i].remainingTime = processes[i].burstTime;
    }

    // Process until all processes are completed
    while (completedProcesses < n) {
        // Enqueue processes that have arrived at the current time
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0) {
                enqueueSRTF(readyQueue, processes[i]);
            }
        }

        // Dequeue the process with the shortest remaining time
        struct Process currentProcess = dequeue(readyQueue);
        if (currentProcess.pid != -1) { // A valid process is dequeued
            if (currentProcess.remainingTime == currentProcess.burstTime) {
                currentProcess.startTime = currentTime;
                currentProcess.responseTime = currentTime - currentProcess.arrivalTime;
                totalResponseTime += currentProcess.responseTime;
            }
            currentTime++;
            currentProcess.remainingTime--;

            if (currentProcess.remainingTime == 0) { // Process has finished execution
                currentProcess.finishTime = currentTime;
                currentProcess.turnaroundTime = currentProcess.finishTime - currentProcess.arrivalTime;
                currentProcess.waitingTime = currentProcess.turnaroundTime - currentProcess.burstTime;
                totalWaitingTime += currentProcess.waitingTime;
                totalTurnaroundTime += currentProcess.turnaroundTime;
                completedProcesses++;
            } else {
                enqueueSRTF(readyQueue, currentProcess); // Re-enqueue the process
            }
        } else { // No process is ready to execute
            currentTime++;
        }
    }

    totalWaitingTime /= n;
    totalTurnaroundTime /= n;
    totalResponseTime /= n;

    printf("Average Waiting Time: %.2f\n", totalWaitingTime);
    printf("Average Turnaround Time: %.2f\n", totalTurnaroundTime);
    printf("Average Response Time: %.2f\n", totalResponseTime);
}

// RR scheduling algorithm
void RR(struct Process processes[], int n, int timeQuantum) {
    struct Queue* readyQueue = createQueue();
    int currentTime = 0;
    float totalWaitingTime = 0, totalTurnaroundTime = 0, totalResponseTime = 0;
    int totalContextSwitch = 0;

    int completedProcesses = 0;

    // Initially, all processes have remaining time equal to burst time
    for (int i = 0; i < n; i++) {
        processes[i].remainingTime = processes[i].burstTime;
    }

    // Process until all processes are completed
    while (completedProcesses < n) {
        // Enqueue processes that have arrived at the current time
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0) {
                enqueue(readyQueue, processes[i]);
            }
        }

        // Dequeue a process and execute it for the time quantum
        struct Process currentProcess = dequeue(readyQueue);
        if (currentProcess.pid != -1) { // A valid process is dequeued
            if (currentProcess.remainingTime == currentProcess.burstTime) {
                currentProcess.startTime = currentTime;
                currentProcess.responseTime = currentTime - currentProcess.arrivalTime;
                totalResponseTime += currentProcess.responseTime;
            }

            if (currentProcess.remainingTime <= timeQuantum) { // Process will finish within time quantum
                currentTime += currentProcess.remainingTime;
                currentProcess.remainingTime = 0;
                currentProcess.finishTime = currentTime;
                currentProcess.turnaroundTime = currentProcess.finishTime - currentProcess.arrivalTime;
                currentProcess.waitingTime = currentProcess.turnaroundTime - currentProcess.burstTime;
                totalWaitingTime += currentProcess.waitingTime;
                totalTurnaroundTime += currentProcess.turnaroundTime;
                completedProcesses++;
            } else { // Process needs more time to finish
                currentTime += timeQuantum;
                currentProcess.remainingTime -= timeQuantum;
                enqueue(readyQueue, currentProcess); // Re-enqueue the process
            }
        } else { // No process is ready to execute
            currentTime++;
        }
    }

    totalWaitingTime /= n;
    totalTurnaroundTime /= n;
    totalResponseTime /= n;

    printf("Average Waiting Time: %.2f\n", totalWaitingTime);
    printf("Average Turnaround Time: %.2f\n", totalTurnaroundTime);
    printf("Average Response Time: %.2f\n", totalResponseTime);
}

int main() {
    int n; // Number of processes
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    // Create an array of processes
    struct Process processes[n];
    for (int i = 0; i < n; i++) {
        printf("Enter Arrival Time for Process %d: ", i + 1);
        scanf("%d", &processes[i].arrivalTime);
        printf("Enter Burst Time for Process %d: ", i + 1);
        scanf("%d", &processes[i].burstTime);
        processes[i].pid = i + 1;
    }

    // Sort the processes based on arrival time (if not already sorted)
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrivalTime > processes[j + 1].arrivalTime) {
                struct Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }

    printf("\n------ FCFS ------\n");
    FCFS(processes, n);

    printf("\n------ SRTF ------\n");
    SRTF(processes, n);

    int timeQuantum;
    printf("\nEnter Time Quantum for RR: ");
    scanf("%d", &timeQuantum);
    printf("\n------ RR ------\n");
    RR(processes, n, timeQuantum);

    return 0;
}