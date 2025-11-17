/**
 * @file CircularBuffer.cpp
 * @brief Implementación de CircularBuffer
 */

#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(int size) : head(nullptr), tail(nullptr),
                                           capacity(size), currentSize(0) {}

bool CircularBuffer::insert(int value) {
    if (isFull()) {
        return false;
    }

    Node* newNode = new Node(value);

    if (isEmpty()) {
        head = tail = newNode;
        newNode->next = newNode;
        newNode->prev = newNode;
    } else {
        newNode->prev = tail;
        newNode->next = head;
        tail->next = newNode;
        head->prev = newNode;
        tail = newNode;
    }

    currentSize++;
    return true;
}

bool CircularBuffer::isFull() const {
    return currentSize >= capacity;
}

bool CircularBuffer::isEmpty() const {
    return currentSize == 0;
}

int CircularBuffer::size() const {
    return currentSize;
}

void CircularBuffer::sort() {
    if (currentSize <= 1) return;

    Node* current = head->next;
    int count = 1;

    while (count < currentSize) {
        Node* key = current;
        int keyData = key->data;
        Node* prev = key->prev;

        while (prev != tail && prev->data > keyData) {
            prev->next->data = prev->data;
            prev = prev->prev;
        }

        if (prev == tail && prev->data > keyData) {
            prev->next->data = prev->data;
            head->data = keyData;
        } else {
            prev->next->data = keyData;
        }

        current = current->next;
        count++;
    }
}

void CircularBuffer::getData(int* arr, int arrSize) {
    if (isEmpty() || arrSize < currentSize) return;

    Node* current = head;
    for (int i = 0; i < currentSize; i++) {
        arr[i] = current->data;
        current = current->next;
    }
}

void CircularBuffer::clear() {
    if (isEmpty()) return;

    Node* current = head;
    for (int i = 0; i < currentSize; i++) {
        Node* temp = current;
        current = current->next;
        delete temp;
    }

    head = tail = nullptr;
    currentSize = 0;
}

void CircularBuffer::print() const {
    if (isEmpty()) {
        std::cout << "Buffer vacío" << std::endl;
        return;
    }

    Node* current = head;
    std::cout << "[";
    for (int i = 0; i < currentSize; i++) {
        std::cout << current->data;
        if (i < currentSize - 1) std::cout << ", ";
        current = current->next;
    }
    std::cout << "]" << std::endl;
}

CircularBuffer::~CircularBuffer() {
    clear();
}