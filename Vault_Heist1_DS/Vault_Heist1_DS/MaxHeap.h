#pragma once

class MaxHeap
{
    int* heap;
    int size;
    int capacity;


public:
    void resize();
    MaxHeap(int initialCapacity);
    ~MaxHeap();

    int getSize();
    int getCapacity();

    void heapifyDown(int index);
    void heapifyUp(int index);
    void buildMaxHeap();
    void insert(int value);
    bool extractMax(int* maxValue);
    bool removeLast(int* removedValue);
    bool getMax(int* maxValue);
    bool swapElements(int index1, int index2);
    bool isMaxHeap();
    void heapSort();
    void loadArray(int values[], int count);
    bool getValueAt(int index, int* value);
};