/**
* @file DataSource.h
 * @brief Clase base abstracta para fuentes de datos
 * @details Define la interfaz para leer datos de diferentes fuentes (serial, archivos)
 */

#ifndef DATASOURCE_H
#define DATASOURCE_H

/**
 * @class DataSource
 * @brief Clase base abstracta para abstracción de fuentes de datos
 * @details Proporciona una interfaz común para leer datos desde diferentes orígenes
 */
class DataSource {
public:
    /**
     * @brief Obtiene el siguiente dato entero de la fuente
     * @return int El siguiente valor entero leído
     * @details Método virtual puro que debe ser implementado por clases derivadas
     */
    virtual int getNext() = 0;

    /**
     * @brief Verifica si hay más datos disponibles
     * @return true si hay más datos, false en caso contrario
     * @details Método virtual puro para verificar disponibilidad de datos
     */
    virtual bool hasMoreData() = 0;

    /**
     * @brief Destructor virtual
     * @details Asegura la correcta destrucción de objetos derivados
     */
    virtual ~DataSource() {}
};

#endif