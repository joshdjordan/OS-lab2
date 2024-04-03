#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100

typedef struct {
    int *array;
    int front;
    int rear;
    int capacity;
} Queue;

Queue* createQueue(int size) {
    Queue *queue = (Queue*)malloc(sizeof(Queue));
    queue -> array = (int*)malloc(size * sizeof(int));
    queue -> capacity = size;
    queue -> front = queue -> rear = -1;
    return queue;
}

int isFull(Queue *queue) {
    return (queue -> rear == queue -> capacity - 1);
}

int isEmpty(Queue *queue) {
    return (queue -> front == -1);
}

void enqueue(Queue *queue, int item) {
    if (isFull(queue)) {
        printf("Queue is full\n");
        return;
    }
  
    if (isEmpty(queue)) {
        queue -> front = 0;
    }
  
    queue -> rear++;
    queue -> array[queue -> rear] = item;
}

int dequeue(Queue *queue) {
    int item;
  
    if (isEmpty(queue)) {
        printf("queue is empty\n");
        return -1;
    }
	
    item = queue -> array[queue -> front];
    
	if (queue -> front == queue -> rear) {
        queue -> front = queue -> rear = -1;
    }
	else {
        queue -> front++;
    }
	
    return item;
}

void display(Queue *queue) {
    if (isEmpty(queue)) {
        printf("queue is empty\n");
        return;
    }
	
    printf("Queue: ");
    for (int i = queue -> front; i <= queue -> rear; i++) {
        printf("%d ", queue -> array[i]);
    }
    printf("\n");
}

int main() {
    Queue *queue = createQueue(MAX_SIZE);
    enqueue(queue, 10);
    enqueue(queue, 20);
    enqueue(queue, 30);
    enqueue(queue, 40);
    display(queue);
    printf("Dequeued item: %d\n", dequeue(queue));
    display(queue);
    printf("Dequeued item: %d\n", dequeue(queue));
    display(queue);
    enqueue(queue, 50);
    enqueue(queue, 60);
    display(queue);
    free(queue->array);
    free(queue);
    return 0;
}
