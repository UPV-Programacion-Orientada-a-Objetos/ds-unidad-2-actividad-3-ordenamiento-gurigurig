/**
 * @file SerialSource.cpp
 * @brief Implementación de SerialSource con lectura REAL de puerto serial
 */

#include "SerialSource.h"
#include <iostream>
#include <sstream>

#ifdef _WIN32
// ============= IMPLEMENTACIÓN WINDOWS =============
SerialSource::SerialSource(const std::string& port, int baudRate)
    : connected(false), dataAvailable(true), timeoutCounter(0) {
    std::wstring widePort(port.begin(), port.end());

    hSerial = CreateFileW(
        widePort.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: No se pudo abrir el puerto " << port << std::endl;
        return;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error: No se pudo obtener el estado del puerto" << std::endl;
        CloseHandle(hSerial);
        return;
    }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error: No se pudo configurar el puerto" << std::endl;
        CloseHandle(hSerial);
        return;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 100;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "Error: No se pudieron configurar los timeouts" << std::endl;
        CloseHandle(hSerial);
        return;
    }

    connected = true;
    std::cout << "Conectado exitosamente" << std::endl;
}

bool SerialSource::readChar(char& c) {
    DWORD bytesRead;
    if (!ReadFile(hSerial, &c, 1, &bytesRead, NULL)) {
        connected = false;
        return false;
    }
    return bytesRead > 0;
}

SerialSource::~SerialSource() {
    if (connected && hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
    }
}

#else
// ============= IMPLEMENTACIÓN LINUX =============
SerialSource::SerialSource(const std::string& port, int baudRate)
    : connected(false), dataAvailable(true), timeoutCounter(0) {
    fd = open(port.c_str(), O_RDWR | O_NOCTTY);

    if (fd == -1) {
        std::cerr << "Error: No se pudo abrir el puerto " << port << std::endl;
        return;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Error: No se pudo obtener atributos del puerto" << std::endl;
        close(fd);
        return;
    }

    speed_t speed;
    switch(baudRate) {
        case 9600:   speed = B9600; break;
        case 115200: speed = B115200; break;
        default:     speed = B9600;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    tty.c_cc[VTIME] = 1;
    tty.c_cc[VMIN] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error: No se pudieron configurar atributos" << std::endl;
        close(fd);
        return;
    }

    connected = true;
    std::cout << "Conectado exitosamente" << std::endl;
}

bool SerialSource::readChar(char& c) {
    int n = read(fd, &c, 1);
    if (n < 0) {
        connected = false;
        return false;
    }
    return n > 0;
}

SerialSource::~SerialSource() {
    if (connected && fd != -1) {
        close(fd);
    }
}
#endif

// ============= FUNCIONES COMUNES =============
bool SerialSource::readLine(std::string& line) {
    if (!connected) return false;

    line.clear();
    char c;
    int emptyReads = 0;
    const int MAX_EMPTY_READS = 100;

    while (emptyReads < MAX_EMPTY_READS) {
        if (readChar(c)) {
            emptyReads = 0;

            if (c == '\n') {
                if (!line.empty()) {
                    return true;
                }
            } else if (c == '\r') {
                // Ignorar \r
                continue;
            } else if ((c >= '0' && c <= '9') || c == '-' || c == ' ') {
                line += c;
            }
        } else {
            emptyReads++;
        }
    }

    return false;
}

int SerialSource::getNext() {
    if (!connected || !dataAvailable) {
        return 0;
    }

    std::string line;
    if (readLine(line)) {
        timeoutCounter = 0;
        try {
            int value = std::stoi(line);
            return value;
        } catch (...) {
            std::cerr << "Error: Dato inválido recibido: " << line << std::endl;
            return getNext();
        }
    } else {
        timeoutCounter++;
        if (timeoutCounter >= MAX_TIMEOUTS) {
            dataAvailable = false;
        }
        return 0;
    }
}

bool SerialSource::hasMoreData() {
    return connected && dataAvailable;
}