/**
* @file FileSource.cpp
 * @brief Implementación de FileSource
 */

#include "FileSource.h"
#include <iostream>

FileSource::FileSource(const std::string& filename) {
    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
    }
}

int FileSource::getNext() {
    int value;
    if (file >> value) {
        return value;
    }
    return 0;
}

bool FileSource::hasMoreData() {
    return file.good() && !file.eof();
}

FileSource::~FileSource() {
    if (file.is_open()) {
        file.close();
    }
}