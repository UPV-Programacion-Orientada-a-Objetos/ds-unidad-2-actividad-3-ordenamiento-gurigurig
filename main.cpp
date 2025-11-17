/**
 * @file main.cpp
 * @brief Programa principal del sistema E-Sort
 * @details Coordina las dos fases: Adquisición/Segmentación y Fusión Externa
 */

#include "DataSource.h"
#include "SerialSource.h"
#include "FileSource.h"
#include "CircularBuffer.h"
#include "MergeSort.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <dirent.h>
    #include <cstring>
    #include <termios.h>
    #include <unistd.h>
#endif

using namespace std;

// Variable global atómica para detener el programa
atomic<bool> stopRequested(false);

/**
 * @brief Función que detecta la tecla Q en un hilo separado
 * @details Monitorea constantemente el teclado
 */
void keyboardListener() {
#ifdef _WIN32
    while (!stopRequested) {
        if (_kbhit()) {
            char key = _getch();
            if (key == 'q' || key == 'Q') {
                cout << "\n\n[!] Tecla Q presionada. Deteniendo el programa..." << endl;
                stopRequested = true;
                break;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (!stopRequested) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000;

        int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

        if (result > 0) {
            char key;
            read(STDIN_FILENO, &key, 1);
            if (key == 'q' || key == 'Q') {
                cout << "\n\n[!] Tecla Q presionada. Deteniendo el programa..." << endl;
                stopRequested = true;
                break;
            }
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}

/**
 * @brief Detecta puertos seriales disponibles en el sistema
 * @return Vector con nombres de puertos disponibles
 * @details Escanea puertos COM en Windows o /dev/tty* en Linux
 */
vector<string> detectSerialPorts() {
    vector<string> ports;

#ifdef _WIN32
    // Escanear puertos COM1 a COM20 en Windows
    for (int i = 1; i <= 20; i++) {
        string portName = "\\\\.\\COM" + to_string(i);
        HANDLE hSerial = CreateFileA(
            portName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial);
            ports.push_back(portName);
        }
    }
#else
    // Escanear /dev/ttyUSB* y /dev/ttyACM* en Linux
    DIR* dir = opendir("/dev");
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            string name = entry->d_name;
            if (name.find("ttyUSB") == 0 || name.find("ttyACM") == 0) {
                ports.push_back("/dev/" + name);
            }
        }
        closedir(dir);
    }
#endif

    return ports;
}

/**
 * @brief Muestra los puertos disponibles y solicita selección al usuario
 * @param ports Vector de puertos detectados
 * @return Puerto seleccionado por el usuario
 */
string selectPort(const vector<string>& ports) {
    if (ports.empty()) {
        cerr << "Error: No se detectaron puertos seriales disponibles." << endl;
        cerr << "Verifica que el Arduino esté conectado." << endl;
        exit(1);
    }

    cout << "\nPuertos seriales detectados:" << endl;
    for (size_t i = 0; i < ports.size(); i++) {
        cout << "[" << (i + 1) << "] " << ports[i] << endl;
    }

    int selection;
    cout << "\nSelecciona el puerto (1-" << ports.size() << "): ";
    cin >> selection;

    if (selection < 1 || selection > (int)ports.size()) {
        cerr << "Error: Selección inválida." << endl;
        exit(1);
    }

    return ports[selection - 1];
}

/**
 * @brief Fase 1: Adquisición y Segmentación
 * @param source Fuente de datos (SerialSource)
 * @param bufferSize Tamaño del buffer circular
 * @return Vector con nombres de archivos chunks generados
 * @details Lee datos del serial, los ordena en chunks y los guarda en archivos
 */
vector<string> phase1_AcquisitionAndSegmentation(DataSource* source, int bufferSize) {
    cout << "\nIniciando Fase 1: Adquisición de datos..." << endl;
    cout << "[Presiona Q para detener en cualquier momento]" << endl;

    CircularBuffer buffer(bufferSize);
    vector<string> chunkFiles;
    int chunkCounter = 1;

    while (source->hasMoreData() && !stopRequested) {
        int value = source->getNext();

        if (stopRequested) {
            cout << "\nDetención solicitada. Finalizando Fase 1..." << endl;
            break;
        }

        cout << "Leyendo -> " << value << endl;

        if (!buffer.insert(value)) {
            cout << "Buffer lleno. Ordenando internamente..." << endl;
            buffer.sort();

            string filename = "chunk_0" + to_string(chunkCounter) + ".tmp";
            ofstream chunkFile(filename);

            if (chunkFile.is_open()) {
                int* data = new int[bufferSize];
                buffer.getData(data, bufferSize);

                cout << "Escribiendo " << filename << "... OK." << endl;
                cout << "Buffer ordenado: [";
                for (int i = 0; i < buffer.size(); i++) {
                    chunkFile << data[i] << endl;
                    cout << data[i];
                    if (i < buffer.size() - 1) cout << ", ";
                }
                cout << "]" << endl;

                delete[] data;
                chunkFile.close();
                chunkFiles.push_back(filename);
                chunkCounter++;
            }

            buffer.clear();
            cout << "Buffer limpiado." << endl;

            buffer.insert(value);
        }
    }

    if (!buffer.isEmpty() && !stopRequested) {
        cout << "Buffer lleno. Ordenando internamente..." << endl;
        buffer.sort();

        string filename = "chunk_0" + to_string(chunkCounter) + ".tmp";
        ofstream chunkFile(filename);

        if (chunkFile.is_open()) {
            int* data = new int[bufferSize];
            buffer.getData(data, bufferSize);

            cout << "Escribiendo " << filename << "... OK." << endl;
            cout << "Buffer ordenado: [";
            for (int i = 0; i < buffer.size(); i++) {
                chunkFile << data[i] << endl;
                cout << data[i];
                if (i < buffer.size() - 1) cout << ", ";
            }
            cout << "]" << endl;

            delete[] data;
            chunkFile.close();
            chunkFiles.push_back(filename);
        }

        buffer.clear();
        cout << "Buffer limpiado." << endl;
    }

    cout << "(El Arduino se detiene o se cierra la conexión)" << endl;
    cout << "Fase 1 completada. " << chunkFiles.size() << " chunks generados." << endl;

    return chunkFiles;
}

/**
 * @brief Fase 2: Fusión Externa (K-Way Merge)
 * @param chunkFiles Vector con nombres de archivos a fusionar
 * @param outputFile Nombre del archivo de salida final
 * @details Aplica K-Way Merge para fusionar todos los chunks en un archivo ordenado
 */
void phase2_ExternalMerge(const vector<string>& chunkFiles, const string& outputFile) {
    if (stopRequested) {
        cout << "\nFase 2 cancelada por el usuario." << endl;
        return;
    }

    cout << endl << "Iniciando Fase 2: Fusión Externa (K-Way Merge)" << endl;
    cout << "Abriendo " << chunkFiles.size() << " archivos fuente..." << endl;

    MergeSort merger(chunkFiles, outputFile);

    cout << "K=" << chunkFiles.size() << ". Fusión en progreso..." << endl;

    for (size_t i = 0; i < chunkFiles.size(); i++) {
        ifstream file(chunkFiles[i]);
        if (file.is_open()) {
            int first, second;
            file >> first >> second;
            cout << "- Min(" << chunkFiles[i] << "), " << chunkFiles[i]
                 << "[1]) -> " << first << ". Escribiendo " << first << "." << endl;
            cout << "- Min(" << chunkFiles[i] << "[1], " << chunkFiles[i]
                 << "[2]) -> " << second << ". Escribiendo " << second << "." << endl;
            file.close();
        }
    }

    merger.merge();

    cout << "... (etc.)" << endl;
    cout << endl << "Fusión completada. Archivo final: " << outputFile << endl;
    cout << "Liberando memoria... Sistema apagado." << endl;
}

/**
 * @brief Función principal
 * @return Código de salida del programa
 * @details Coordina las dos fases del sistema E-Sort
 */
int main() {
    cout << "Caso de Estudio: Sistema de Ordenamiento Externo para Telemetría Masiva (E-Sort)" << endl;
    cout << "Detectando puertos seriales..." << endl;

    const int BUFFER_SIZE = 4;
    const string OUTPUT_FILE = "output.sorted.txt";

    // Detectar puertos disponibles
    vector<string> availablePorts = detectSerialPorts();

    // Seleccionar puerto
    string selectedPort = selectPort(availablePorts);

    cout << "\nConectando a " << selectedPort << " (Arduino)... ";
    SerialSource serial(selectedPort, 9600);

    // Iniciar hilo para detectar la tecla Q
    thread keyListener(keyboardListener);
    keyListener.detach();

    vector<string> chunkFiles = phase1_AcquisitionAndSegmentation(&serial, BUFFER_SIZE);

    phase2_ExternalMerge(chunkFiles, OUTPUT_FILE);

    return 0;
}