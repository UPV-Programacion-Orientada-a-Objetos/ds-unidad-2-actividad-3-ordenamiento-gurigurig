/**
 * @file SerialSource.h
 * @brief Clase concreta para lectura de datos desde puerto serial REAL
 * @details Hereda de DataSource e implementa lectura desde Arduino usando APIs nativas
 */

#ifndef SERIALSOURCE_H
#define SERIALSOURCE_H

#include "DataSource.h"
#include <string>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <termios.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/time.h>
#endif

/**
 * @class SerialSource
 * @brief Fuente de datos que lee desde puerto serial REAL (COM/tty)
 * @details Lee datos enteros línea por línea desde un puerto COM físico
 */
class SerialSource : public DataSource {
private:
#ifdef _WIN32
    HANDLE hSerial;        ///< Handle del puerto serial en Windows
#else
    int fd;                ///< File descriptor del puerto serial en Linux
#endif
    std::string buffer;    ///< Buffer para acumular caracteres
    bool connected;        ///< Estado de conexión
    bool dataAvailable;    ///< Indica si hay datos disponibles
    int timeoutCounter;    ///< Contador de timeouts consecutivos
    static const int MAX_TIMEOUTS = 50; ///< Máximo de timeouts antes de considerar que no hay más datos

    /**
     * @brief Lee un carácter del puerto serial
     * @param c Referencia donde se almacenará el carácter leído
     * @return true si se leyó correctamente, false en caso de error
     */
    bool readChar(char& c);

    /**
     * @brief Lee una línea completa del puerto serial con timeout
     * @param line Referencia donde se almacenará la línea leída
     * @return true si se leyó una línea válida, false si timeout o error
     */
    bool readLine(std::string& line);

public:
    /**
     * @brief Constructor que abre el puerto serial REAL
     * @param port Nombre del puerto serial (ej: "COM3" en Windows o "/dev/ttyUSB0" en Linux)
     * @param baudRate Velocidad de comunicación (por defecto 9600)
     * @details Conecta al puerto físico usando WinAPI o POSIX
     */
    SerialSource(const std::string& port, int baudRate = 9600);

    /**
     * @brief Lee y devuelve el siguiente entero del serial
     * @return int Siguiente valor leído del puerto
     * @details Lee caracteres hasta encontrar '\n' y convierte a entero
     */
    int getNext() override;

    /**
     * @brief Verifica si hay más datos disponibles en el serial
     * @return true si hay datos disponibles, false si timeout o error
     */
    bool hasMoreData() override;

    /**
     * @brief Destructor que cierra la conexión serial
     */
    ~SerialSource();
};

#endif