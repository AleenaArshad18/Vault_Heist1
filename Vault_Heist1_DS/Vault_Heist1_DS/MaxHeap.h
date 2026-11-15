#pragma once

class MaxHeap
{
    int* heap;
    int size;
    int capacity;

    void resize();

public:
    MaxHeap(int initialCapacity);
    ~MaxHeap();

    int getSize();
    int getCapacity();

    void heapifyDown(int index);
    void buildMaxHeap();
};