/**
 * @file MergeSort.h
 * @brief Implementación del algoritmo K-Way Merge para fusión externa
 * @details Fusiona múltiples archivos ordenados en un único archivo final
 */

#ifndef MERGESORT_H
#define MERGESORT_H

#include "DataSource.h"
#include "FileSource.h"
#include <vector>
#include <string>
#include <fstream>

/**
 * @struct MergeElement
 * @brief Elemento usado durante el proceso de merge
 * @details Almacena un valor y el índice de su fuente
 */
struct MergeElement {
    int value;          ///< Valor del elemento
    int sourceIndex;    ///< Índice de la fuente de datos

    /**
     * @brief Constructor del elemento
     * @param val Valor a almacenar
     * @param idx Índice de la fuente
     */
    MergeElement(int val, int idx) : value(val), sourceIndex(idx) {}
};

/**
 * @class MergeSort
 * @brief Implementa K-Way Merge para ordenamiento externo
 * @details Fusiona K archivos ordenados usando un heap manual
 */
class MergeSort {
private:
    std::vector<FileSource*> sources;  ///< Vector de fuentes de datos (archivos)
    std::ofstream outputFile;          ///< Archivo de salida

    /**
     * @brief Encuentra el índice del elemento mínimo entre las fuentes activas
     * @param elements Vector de elementos actuales de cada fuente
     * @param active Vector indicando qué fuentes aún tienen datos
     * @return Índice de la fuente con el elemento mínimo
     */
    int findMinIndex(const std::vector<int>& elements, const std::vector<bool>& active);

public:
    /**
     * @brief Constructor
     * @param chunkFiles Vector con nombres de archivos a fusionar
     * @param outputFileName Nombre del archivo de salida
     * @details Abre todos los archivos fuente y el archivo de salida
     */
    MergeSort(const std::vector<std::string>& chunkFiles, const std::string& outputFileName);

    /**
     * @brief Ejecuta el algoritmo K-Way Merge
     * @details Lee el primer elemento de cada fuente, selecciona el mínimo,
     *          lo escribe en el archivo de salida, y avanza esa fuente
     */
    void merge();

    /**
     * @brief Destructor que libera recursos
     */
    ~MergeSort();
};

#endif