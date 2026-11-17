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

void MaxHeap::heapifyUp(int index){
    if (index == 0) {
        return;
    }

    int parent = (index - 1) / 2;

    if (heap[index] > heap[parent]){
        int temp = heap[index];
        heap[index] = heap[parent];
        heap[parent] = temp;

        heapifyUp(parent);
    }
}

void MaxHeap::insert(int value)
{
    if (size == capacity)
    {
        resize();
    }

    heap[size] = value;

    size++;

    heapifyUp(size - 1);
}

bool MaxHeap::extractMax(int* maxValue)
{
    if (size == 0)
    {
        return false;
    }

    *maxValue = heap[0];

    heap[0] = heap[size - 1];

    size--;

    if (size > 0)
    {
        heapifyDown(0);
    }

    return true;
}

bool MaxHeap::getMax(int* maxValue)
{
    if (size == 0)
    {
        return false;
    }

    *maxValue = heap[0];

    return true;
}

bool MaxHeap::swapElements(int index1, int index2)
{
    if (index1 < 0 || index1 >= size ||
        index2 < 0 || index2 >= size)
    {
        return false;
    }

    int temp = heap[index1];
    heap[index1] = heap[index2];
    heap[index2] = temp;

    return true;
}

bool MaxHeap::isMaxHeap()
{
    for (int i = 0; i < size / 2; i++)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < size && heap[i] < heap[left])
        {
            return false;
        }

        if (right < size && heap[i] < heap[right])
        {
            return false;
        }
    }

    return true;
}

void MaxHeap::heapSort()
{
    buildMaxHeap();

    int originalSize = size;

    for (int i = size - 1; i > 0; i--)
    {
        int temp = heap[0];
        heap[0] = heap[i];
        heap[i] = temp;

        size--;

        heapifyDown(0);
    }

    size = originalSize;
}

void MaxHeap::loadArray(int values[], int count)
{
    if (count < 0)
    {
        return;
    }

    while (capacity < count)
    {
        resize();
    }

    for (int i = 0; i < count; i++)
    {
        heap[i] = values[i];
    }

    size = count;
}

bool MaxHeap::getValueAt(int index, int* value)
{
    if (index < 0 || index >= size)
    {
        return false;
    }

    *value = heap[index];

    return true;
}