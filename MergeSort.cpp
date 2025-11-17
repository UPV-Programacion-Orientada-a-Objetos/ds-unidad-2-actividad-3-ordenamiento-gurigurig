/**
* @file MergeSort.cpp
 * @brief Implementación de MergeSort
 */

#include "MergeSort.h"
#include <iostream>
#include <limits>

MergeSort::MergeSort(const std::vector<std::string>& chunkFiles,
                     const std::string& outputFileName) {
    for (const auto& filename : chunkFiles) {
        sources.push_back(new FileSource(filename));
    }

    outputFile.open(outputFileName);
    if (!outputFile.is_open()) {
        std::cerr << "Error: No se pudo crear el archivo de salida "
                  << outputFileName << std::endl;
    }
}

int MergeSort::findMinIndex(const std::vector<int>& elements,
                            const std::vector<bool>& active) {
    int minIndex = -1;
    int minValue = std::numeric_limits<int>::max();

    for (size_t i = 0; i < elements.size(); i++) {
        if (active[i] && elements[i] < minValue) {
            minValue = elements[i];
            minIndex = i;
        }
    }

    return minIndex;
}

void MergeSort::merge() {
    int K = sources.size();
    std::vector<int> currentElements(K);
    std::vector<bool> active(K);

    for (int i = 0; i < K; i++) {
        if (sources[i]->hasMoreData()) {
            currentElements[i] = sources[i]->getNext();
            active[i] = true;
        } else {
            active[i] = false;
        }
    }

    while (true) {
        int minIndex = findMinIndex(currentElements, active);

        if (minIndex == -1) {
            break;
        }

        outputFile << currentElements[minIndex] << std::endl;

        if (sources[minIndex]->hasMoreData()) {
            currentElements[minIndex] = sources[minIndex]->getNext();
        } else {
            active[minIndex] = false;
        }
    }
}

MergeSort::~MergeSort() {
    for (auto source : sources) {
        delete source;
    }
    sources.clear();

    if (outputFile.is_open()) {
        outputFile.close();
    }
}