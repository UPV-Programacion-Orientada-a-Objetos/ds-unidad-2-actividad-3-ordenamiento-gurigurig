/**
* @file FileSource.h
 * @brief Clase concreta para lectura de datos desde archivos
 * @details Hereda de DataSource e implementa lectura desde archivos temporales (.tmp)
 */

#ifndef FILESOURCE_H
#define FILESOURCE_H

#include "DataSource.h"
#include <fstream>
#include <string>

/**
 * @class FileSource
 * @brief Fuente de datos que lee desde archivos
 * @details Lee datos enteros desde archivos temporales (chunks)
 */
class FileSource : public DataSource {
private:
    std::ifstream file; ///< Stream del archivo

public:
    /**
     * @brief Constructor que abre un archivo
     * @param filename Nombre del archivo a leer (ej: "chunk_XX.tmp")
     * @details Abre el archivo especificado para lectura
     */
    FileSource(const std::string& filename);

    /**
     * @brief Lee y devuelve el siguiente entero del archivo
     * @return int Siguiente valor leído
     * @details Lee una línea y la convierte a entero
     */
    int getNext() override;

    /**
     * @brief Verifica si hay más datos en el archivo
     * @return true si hay más datos disponibles, false en EOF
     */
    bool hasMoreData() override;

    /**
     * @brief Destructor que cierra el archivo
     */
    ~FileSource();
};

#endif