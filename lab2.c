#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONTEXT_SWITCH_TIME 0.5

/*
Notes:  some of the sources used to aid in this assignemt were chatGPT, stackoverflow, and some geeksforgeeks.
        all code was written by me however some of the algorithms/methods were inspired/used from some of these
        sites along with some help from some classmates. 
*/

struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int remainingBurst;
    int priority;
    double completionTime;
    double waitingTime;
    double turnaroundTime;
    double responseTime;
    int numContext;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                 QUEUE IMPLEMENTATION                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Node {
    struct Process data;
    struct Node* next;
} Node;

typedef struct {
    Node* front;
    Node* rear;
    int size;
    int capacity;
} Queue;

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (queue == NULL) {
        printf("memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    queue -> front = queue -> rear = NULL;
    queue -> size = 0;
    queue -> capacity = 0; // Not really needed, but kept for consistency
    return queue;
}

int isEmpty(Queue* queue) {
    return (queue -> front == NULL);
}

void enqueue(Queue* queue, struct Process item) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode -> data = item;
    newNode -> next = NULL;
  
    if (isEmpty(queue)) {
        queue -> front = newNode;
    } else {
        queue -> rear -> next = newNode;
    }
    queue -> rear = newNode;
    queue -> size++;
}

struct Process dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        printf("queue is empty\n");
        exit(EXIT_FAILURE);
    }
    Node* temp = queue -> front;
    struct Process item = temp -> data;
    queue -> front = queue->front -> next;
    if (queue -> front == NULL) {
        queue -> rear = NULL;
    }
    free(temp);
    queue -> size--;
    return item;
}

// void display(Queue* queue) {
//     if (isEmpty(queue)) {
//         printf("queue is empty\n");
//         return;
//     }
//     printf("Queue: ");
    
//     Node* current = queue -> front;

    // while (current != NULL) {
    //     printf("\n\nPid %d:\n----------------------\n", current -> data.pid);
    //     printf("Arrival Time: %d\n", current -> data.arrivalTime);
    //     printf("CPU-Burst: %d\n", current -> data.burstTime);
    //     printf("Priority: %d\n", current -> data.priority);
    //     printf("Finish: %.0f\n", current -> data.completionTime);
    //     printf("Waiting Time: %.0f\n", current -> data.waitingTime);
    //     printf("Turn Around: %.0f\n", current -> data.turnaroundTime);
    //     printf("Response Time: %.0f\n", current -> data.responseTime);
    //     printf("No. of Context: %d\n", current -> data.numContext);
    //     printf("----------------------\n");
        
    //     current = current -> next;
    // }
    // printf("\n");

    // while (current != NULL) {
    //     printf("\tPid\t|\tArrival Time\t|\tCPU-Burst\t|\tPriority\t|\t \n");
    // }
// }

void display(Queue *queue, char *scheduler) {
    if (isEmpty(queue)) {
        printf("queue is empty\n");
        return;
    }
    
    Node* current = queue -> front;
    
    if (!strcmp(scheduler, "0")) {
        printf("*********************************************************************\n");
        printf("************ Scheduling algorithm: FCFS *******************************\n");
        printf("*********************************************************************\n");
        printf("Pid\tArrival\tCPU-Burst\tFinish\tWaiting\tTurnaround\tresponse\tNo. of\n");
        printf("\tTime\tTime\t\ttime\ttime\ttime\t\ttime\tContext\n");
        printf("\t\t\t\t\t\t\t\t\tSwitch\n");
        printf("---------------------------------------------------------------------\n");
    }

    else if (!strcmp(scheduler, "1")) {
        printf("*********************************************************************\n");
        printf("************ Scheduling algorithm: SRTF *******************************\n");
        printf("*********************************************************************\n");
        printf("pid\tarrival\tCPU-burst\tfinish\twaiting\tturnaround\tresponse\tNo. of\n");
        printf("\ttime\ttime\t\ttime\ttime\ttime\t\ttime\tContext\n");
        printf("\t\t\t\t\t\t\t\t\tSwitch\n");
        printf("---------------------------------------------------------------------\n");
    } else {
        printf("*********************************************************************\n");
        printf("************ Scheduling algorithm: RR *******************************\n");
        printf("*********************************************************************\n");
        printf("pid\tarrival\tCPU-burst\tfinish\twaiting\tturnaround\tresponse\tNo. of\n");
        printf("\ttime\ttime\t\ttime\ttime\ttime\t\ttime\tContext\n");
        printf("\t\t\t\t\t\t\t\t\tSwitch\n");
        printf("---------------------------------------------------------------------\n");
    }

    while (current != NULL) {
        printf("%d\t%d\t%d\t\t%.1f\t%.1f\t%.1f\t\t%.1f\t%d\n", current -> data.pid, current -> data.arrivalTime,
        current -> data.burstTime, current -> data.completionTime, current -> data.waitingTime, current -> data.turnaroundTime,
        current -> data.responseTime, current -> data.numContext);

        current = current -> next;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                   PROCESS IMPLEMENTATION                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void readFile(struct Process process[], int maxConnections) {
    FILE* processData = fopen("input10", "r");
    // FILE* processData = fopen("input100", "r");
    if (processData == NULL) {
        perror("error opening file");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < maxConnections; i++) {
        if (fscanf(processData, "%d %d %d %d", &process[i].pid, &process[i].arrivalTime, &process[i].burstTime, &process[i].priority) != 4) {
            break; // Stop reading if fscanf fails
        }
    }
    fclose(processData);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          FIRST-COME-FIRST-SERVE IMPLEMENTATION                                             //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void firstComeFirstServe(struct Process process[], int maxConnections, char *scheduler) {
    Queue *queue = createQueue();
    Queue *completedQ = createQueue();

    int currentTime = 0;
    int ttlWaitingTime = 0;
    int ttlTurnaroundTime = 0;
    int ttlResponseTime =0;
    int ttlContextSwitches = 0;
    int complProcess = 0;
    int ttlBurstTime = 0;
    int i = 0;
    double avgWaitingTime;
    double avgTurnaroundTime;
    double avgResponseTime;
    double avgBurstTime;

    // cycle through each process
    while (!isEmpty(queue) || i < maxConnections) {
        // add each process to the queue that has arrived
        while (i < maxConnections && process[i].arrivalTime <= currentTime) {
            enqueue(queue, process[i]);
            i++;
        }

        // if the queue is empty -> set 'currentTime' to 'arrivalTime' of the next process in the list
        if (isEmpty(queue)) {
            currentTime = process[i].arrivalTime;
            continue; // cycles back to the inner while loop to add more processes to the queue to begin calculations
        }

        // beginning scheduling
        struct Process current = dequeue(queue);
        current.responseTime = currentTime - current.arrivalTime;

        // updated 'currentTime' based on the 'burstTime' of the current process that was executed
        currentTime += current.burstTime;

        // updating values for current process
        current.completionTime = currentTime;
        current.waitingTime = currentTime - current.arrivalTime - current.burstTime;
        current.turnaroundTime = current.completionTime - current.arrivalTime;
        current.numContext = 0;

        // updating values and making final calculations via assignment requirements
        ttlWaitingTime += current.waitingTime;
        ttlTurnaroundTime += current.turnaroundTime;
        ttlResponseTime += current.responseTime;
        ttlBurstTime += current.burstTime;
        complProcess++;
        current.numContext = ttlContextSwitches;

        // adding process to completed process queue to reference later for display
        enqueue(completedQ, current);
    }

    // average calculations
    avgBurstTime = (double)ttlBurstTime / complProcess;
    avgWaitingTime = (double)ttlWaitingTime / complProcess;
    avgTurnaroundTime = (double)ttlTurnaroundTime / complProcess;
    avgResponseTime = (double)ttlResponseTime / complProcess;
    
    // display process info
    display(completedQ, scheduler);
    
    // displaying average results
    // printf("DISPLAYING AVERAGES...\n");
    // printf("----------------------------------------------------\n");
    // printf("Average CPU Burst Time: %.2f\n", avgBurstTime);
    // printf("Average Waiting Time: %.2f\n", avgWaitingTime);
    // printf("Average Turnaround Time: %.2f\n", avgTurnaroundTime);
    // printf("Average Response Time: %.2f\n", avgResponseTime);
    // printf("Total Number of Context Switches performed: %d\n", ttlContextSwitches);

    printf("---------------------------------------------------------------------\n");
    printf("Average CPU burst time = %.2f ms, Average waiting time = %.2f ms\n", avgBurstTime, avgWaitingTime);
    printf("Average turn around time = %.2f ms, Average response time = %.2f ms\n", avgTurnaroundTime, avgResponseTime);
    printf("Total No. of Context Switching Performed = %d\n", ttlContextSwitches);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                   SHORTEST REMAINING TIME FIRST (SRTF) IMPLEMENTATION                                      //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void shortestRemainingTimeFirst(struct Process process[], int maxConnections, char *scheduler) {
    Queue* queue = createQueue();
    Queue* completedQ = createQueue();

    int currentTime = 0;
    int complProcess = 0;
    int ttlContextSwitches = 0;
    int ttlBurstTime = 0;
    double ttlWaitingTime = 0;
    double ttlTurnaroundTime = 0;
    double ttlResponseTime = 0;
    double avgBurstTime;
    double avgWaitingTime;
    double avgTurnaroundTime;
    double avgResponseTime;
    

    int i = 0;
    int index = -1; // Index of the last processed process

    while (complProcess < maxConnections) {
        // Add arriving processes to the queue
        while (i < maxConnections && process[i].arrivalTime <= currentTime) {
            enqueue(queue, process[i]);
            i++;
        }

        if (!isEmpty(queue)) {
            struct Process current = dequeue(queue);
            int executionTime = current.remainingBurst; // Execute the remaining burst time
            currentTime += executionTime;
            current.remainingBurst = 0; // Process is completed
            current.completionTime = currentTime;
            current.turnaroundTime = current.completionTime - current.arrivalTime;
            current.waitingTime = current.turnaroundTime - current.burstTime;
            current.responseTime = current.waitingTime;

            ttlWaitingTime += current.waitingTime;
            ttlTurnaroundTime += current.turnaroundTime;
            ttlResponseTime += current.responseTime;
            complProcess++;

            enqueue(completedQ, current);

            // Update last processed index
            index = current.pid - 1;
        } else {
            // Find the next arrival time after the last processed process
            int nextArrivalTime = 9999;
            for (int j = index + 1; j < maxConnections; j++) {
                if (process[j].arrivalTime < nextArrivalTime) {
                    nextArrivalTime = process[j].arrivalTime;
                }
            }
            currentTime = nextArrivalTime;
        }
    }

    // Calculate averages
    avgBurstTime = ttlBurstTime / maxConnections;
    avgWaitingTime = ttlWaitingTime / maxConnections;
    avgTurnaroundTime = ttlTurnaroundTime / maxConnections;
    avgResponseTime = ttlResponseTime / maxConnections;

    // Display process info
    display(completedQ, scheduler);

    // Display average results
    // printf("DISPLAYING AVERAGES...\n");
    // printf("----------------------------------------------------\n");
    // printf("Average Burst Time: %.2f\n", avgBurstTime);
    // printf("Average Waiting Time: %.2f\n", avgWaitingTime);
    // printf("Average Turnaround Time: %.2f\n", avgTurnaroundTime);
    // printf("Average Response Time: %.2f\n", avgResponseTime);
    // printf("Total Number of Context Switches: %d\n", ttlContextSwitches);

    printf("---------------------------------------------------------------------\n");
    printf("Average CPU burst time = %.2f ms, Average waiting time = %.2f ms\n", avgBurstTime, avgWaitingTime);
    printf("Average turn around time = %.2f ms, Average response time = %.2f ms\n", avgTurnaroundTime, avgResponseTime);
    printf("Total No. of Context Switching Performed = %d\n", ttlContextSwitches);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            ROUND-ROBIN (RR) IMPLEMENTATION                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void roundRobin(struct Process process[], int maxConnections, char *scheduler, int quantumTime) {
    Queue *queue = createQueue();
    Queue *completedQ = createQueue();

    int ttlContextSwitches = 0;
    int complProcess = 0;
    int currentTime = 0;
    int ttlWaitingTime = 0;
    int ttlTurnaroundTime = 0;
    int ttlBurstTime = 0;
    int ttlResponseTime = 0;
    double avgBurstTime;
    double avgWaitingTime;
    double avgTurnaroundTime;
    double avgResponseTime;

    // sets 'remainingBurst' for all processes
    for (int i = 0; i < maxConnections; i++) {
        process[i].remainingBurst = process[i].burstTime;
    }

    int i = 0;
    int index = -1; // var to keep track of 'i' in following for loop to avoid repeats

    // cycle through the processes adding those to the queue that arrive
    while (complProcess < maxConnections) {
        for (int i = index + 1; i < maxConnections; i++) {
            if (process[i].arrivalTime <= currentTime && process[i].remainingBurst > 0) {
                enqueue(queue, process[i]);
                index = i;
            }
        }

        // if queue is NOT empty
        if (!isEmpty(queue)) {
            struct Process current = dequeue(queue);
            int executionTime = (current.remainingBurst < quantumTime) ? current.remainingBurst : quantumTime;
            current.remainingBurst -= executionTime;
            currentTime += (double)executionTime;

            // if process is completed
            if (current.remainingBurst == 0) {
                current.completionTime = currentTime;
                current.turnaroundTime = current.completionTime - current.arrivalTime;
                current.waitingTime = current.turnaroundTime - current.burstTime;
                current.responseTime = current.waitingTime; // For RR, response time is same as waiting time
                complProcess++;
                enqueue(completedQ, current);
            } else {
                enqueue(queue, current);
                currentTime += CONTEXT_SWITCH_TIME; // Context switch time
                ttlContextSwitches++;
            }
            ttlWaitingTime += current.waitingTime;
            ttlTurnaroundTime += current.turnaroundTime;
            ttlResponseTime += current.responseTime;
            ttlBurstTime += current.burstTime;
        } else {
            currentTime = process[i].arrivalTime;
        }
    }

    // calculating averages
    avgBurstTime = (double)ttlBurstTime / maxConnections;
    avgWaitingTime = (double)ttlWaitingTime / maxConnections;
    avgTurnaroundTime = (double)ttlTurnaroundTime / maxConnections;
    avgResponseTime = (double)ttlResponseTime / maxConnections;

    // displaying process info
    display(completedQ, scheduler);

    // printf("DISPLAYING AVERAGES...\n");
    // printf("----------------------------------------------------\n");
    // printf("Average Burst Time: %.2f\n", avgBurstTime);
    // printf("Average Waiting Time: %.2f\n", avgWaitingTime);
    // printf("Average Turnaround Time: %.2f\n", avgTurnaroundTime);
    // printf("Average Response Time: %.2f\n", avgResponseTime);
    // printf("Total Number of Context Switches: %d\n", ttlContextSwitches);

    printf("---------------------------------------------------------------------\n");
    printf("Average CPU burst time = %.2f ms, Average waiting time = %.2f ms\n", avgBurstTime, avgWaitingTime);
    printf("Average turn around time = %.2f ms, Average response time = %.2f ms\n", avgTurnaroundTime, avgResponseTime);
    printf("Total No. of Context Switching Performed = %d\n", ttlContextSwitches);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                      DRIVER PROGRAM                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    Queue* queue = createQueue();

    struct Process process[10];
    int maxConnections = 10;
    int quantumTime = atoi(argv[2]);
    char scheduler[10];
    strcpy(scheduler, argv[1]);
    
    // obtaining all CPU information
    readFile(process, maxConnections);


    // executing FCFS scheduler
    if (!strcmp(argv[1], "0")) {
        firstComeFirstServe(process, maxConnections, scheduler);
    }

    // executing SRTF scheduler
    // if (!strcmp(argv[1], "1")) {
    //     // shortestRunTimeFirst(process, maxConnections);
    // }

    // executing RR scheduler
    // int quantumTime = argv[2];
    
    if (!strcmp(argv[1], "2")) {
        // roundRobin(process, maxConnections, quantumTime);
        roundRobin(process, maxConnections, scheduler, quantumTime);
    }

    return 0;
}

// test main
// int main() {
//     Queue* queue = createQueue();

//     struct Process process[10];
//     int maxConnections = 10;
    
//     // obtaining all CPU information
//     readFile(process, maxConnections);

//     // executing FCFS scheduler
//     // firstComeFirstServe(process, maxConnections);

//     // executing SRTF scheduler
//     shortestRunTimeFirst(process, maxConnections);

//     // executing RR scheduler
//     // int quantumTime = 4;
//     // roundRobin(process, maxConnections, quantumTime);

//     return 0;
// }