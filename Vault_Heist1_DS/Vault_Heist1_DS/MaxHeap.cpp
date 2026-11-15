#include "MaxHeap.h"

MaxHeap::MaxHeap(int initialCapacity){
    if (initialCapacity <= 0){
        initialCapacity = 5;
    }

    capacity = initialCapacity;
    size = 0;

    heap = new int[capacity];
}

MaxHeap::~MaxHeap(){
    delete[] heap;
}

void MaxHeap::resize(){
    int newCapacity = capacity * 2;

    int* newHeap = new int[newCapacity];

    for (int i = 0; i < size; i++){
        newHeap[i] = heap[i];
    }

    delete[] heap;

    heap = newHeap;
    capacity = newCapacity;
}

int MaxHeap::getSize(){
    return size;
}

int MaxHeap::getCapacity(){
    return capacity;
}

void MaxHeap::heapifyDown(int index){
    int largest = index;

    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < size && heap[left] > heap[largest]){
        largest = left;
    }

    if (right < size && heap[right] > heap[largest]){
        largest = right;
    }

    if (largest != index){
        int temp = heap[index];
        heap[index] = heap[largest];
        heap[largest] = temp;

        heapifyDown(largest);
    }
}

void MaxHeap::buildMaxHeap(){
    for (int i = size / 2 - 1; i >= 0; i--){
        heapifyDown(i);
    }
}