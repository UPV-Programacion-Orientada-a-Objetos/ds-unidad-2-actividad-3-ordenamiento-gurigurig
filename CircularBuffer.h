/**
 * @file CircularBuffer.h
 * @brief Lista circular doblemente enlazada de tamaño fijo
 * @details Implementa un buffer circular para almacenar temporalmente los datos entrantes
 */

#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <iostream>

/**
 * @struct Node
 * @brief Nodo de la lista doblemente enlazada
 * @details Almacena un dato entero y punteros a nodos anterior y siguiente
 */
struct Node {
    int data;      ///< Dato almacenado en el nodo
    Node* next;    ///< Puntero al siguiente nodo
    Node* prev;    ///< Puntero al nodo anterior

    /**
     * @brief Constructor del nodo
     * @param value Valor a almacenar
     */
    Node(int value) : data(value), next(nullptr), prev(nullptr) {}
};

/**
 * @class CircularBuffer
 * @brief Buffer circular de tamaño fijo implementado con lista doblemente enlazada
 * @details Maneja la inserción y ordenamiento de datos en memoria fija
 */
class CircularBuffer {
private:
    Node* head;           ///< Puntero al primer nodo
    Node* tail;           ///< Puntero al último nodo
    int capacity;         ///< Capacidad máxima del buffer
    int currentSize;      ///< Tamaño actual del buffer

public:
    /**
     * @brief Constructor que inicializa el buffer
     * @param size Tamaño fijo del buffer (ej: 1000 elementos)
     * @details Crea un buffer vacío con capacidad especificada
     */
    CircularBuffer(int size);

    /**
     * @brief Inserta un dato en el buffer
     * @param value Valor a insertar
     * @details Si el buffer está lleno, retorna false; si no, inserta al final
     * @return true si se insertó, false si el buffer está lleno
     */
    bool insert(int value);

    /**
     * @brief Verifica si el buffer está lleno
     * @return true si alcanzó la capacidad máxima
     */
    bool isFull() const;

    /**
     * @brief Verifica si el buffer está vacío
     * @return true si no hay elementos
     */
    bool isEmpty() const;

    /**
     * @brief Obtiene el tamaño actual del buffer
     * @return Número de elementos almacenados
     */
    int size() const;

    /**
     * @brief Ordena el contenido del buffer usando Insertion Sort
     * @details Ordena los datos de menor a mayor in-place
     */
    void sort();

    /**
     * @brief Obtiene todos los datos del buffer en un arreglo
     * @param arr Arreglo donde se copiarán los datos
     * @param arrSize Tamaño del arreglo (debe ser >= currentSize)
     * @details Copia los datos ordenados al arreglo proporcionado
     */
    void getData(int* arr, int arrSize);

    /**
     * @brief Limpia el buffer liberando toda la memoria
     * @details Elimina todos los nodos y reinicia el buffer
     */
    void clear();

    /**
     * @brief Imprime el contenido del buffer (para depuración)
     */
    void print() const;

    /**
     * @brief Destructor que libera toda la memoria
     */
    ~CircularBuffer();
};

#endif