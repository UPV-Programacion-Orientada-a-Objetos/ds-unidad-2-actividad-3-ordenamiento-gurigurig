# Sistema de Ordenamiento Externo para Telemetría Masiva (E-Sort)

## Descripción

Programa en C++ que lee datos desde un Arduino conectado por puerto serial, los ordena usando una lista circular doblemente enlazada, y genera un archivo final ordenado usando el algoritmo K-Way Merge.

## Estructura del Proyecto

```
DataSource.h          - Clase base abstracta
SerialSource.h/cpp    - Lectura desde puerto serial
FileSource.h/cpp      - Lectura desde archivos
CircularBuffer.h/cpp  - Lista circular doblemente enlazada
MergeSort.h/cpp       - Algoritmo K-Way Merge
main.cpp              - Programa principal
Arduino.ino           - Código para Arduino
```

## Requisitos

- Compilador C++ con soporte para C++11
- Arduino conectado por USB
- Arduino IDE (para cargar el sketch)

**Windows:** MinGW o Visual Studio
**Linux:** GCC/G++

## Instalación

### 1. Preparar el Arduino

Abre Arduino IDE y carga este código:

```cpp
void setup() {
  Serial.begin(9600);
  
  int readings[] = {105, 5, 210, 99, 1, 500, 20, 15};
  int numReadings = 8;
  
  for (int i = 0; i < numReadings; i++) {
    Serial.println(readings[i]);
    delay(100);
  }
}

void loop() {
}
```

Importante: Cierra el monitor serial después de cargar el código.

### 2. Compilar el Proyecto

**Windows:**
```bash
g++ -std=c++11 -o esort.exe main.cpp SerialSource.cpp FileSource.cpp CircularBuffer.cpp MergeSort.cpp
```

**Linux:**
```bash
g++ -std=c++11 -pthread -o esort main.cpp SerialSource.cpp FileSource.cpp CircularBuffer.cpp MergeSort.cpp
```

**Permisos en Linux:**
```bash
sudo chmod 666 /dev/ttyUSB0
```

### 3. Ejecutar

```bash
./esort          # Linux
esort.exe        # Windows
```

## Uso

1. Conecta el Arduino al puerto USB
2. Carga el sketch en el Arduino
3. Cierra el monitor serial
4. Ejecuta el programa
5. Selecciona el puerto cuando te lo pida
6. El programa lee los datos y genera output.sorted.txt

Durante la ejecución puedes presionar Q para detener el programa.

## Cómo Funciona

### Fase 1: Adquisición y Segmentación

1. Lee datos del puerto serial uno por uno
2. Los almacena en un buffer circular de tamaño fijo (4 elementos)
3. Cuando el buffer se llena:
   - Ordena los datos con Insertion Sort
   - Guarda el resultado en un archivo chunk_XX.tmp
   - Limpia el buffer y continúa leyendo

### Fase 2: Fusión Externa

1. Abre todos los archivos chunk_XX.tmp
2. Aplica K-Way Merge:
   - Lee el primer elemento de cada archivo
   - Selecciona el menor de todos
   - Lo escribe en output.sorted.txt
   - Avanza en el archivo correspondiente
3. Repite hasta procesar todos los datos

## Ejemplo de Salida

```
Detectando puertos seriales...

Puertos seriales detectados:
[1] \\.\COM3

Selecciona el puerto (1): 1

Conectando a \\.\COM3 (Arduino)... Conectado exitosamente

Iniciando Fase 1: Adquisición de datos...
Leyendo -> 105
Leyendo -> 5
Leyendo -> 210
Leyendo -> 99
Buffer lleno. Ordenando internamente...
Escribiendo chunk_01.tmp... OK.
Buffer ordenado: [5, 99, 105, 210]
Buffer limpiado.

Leyendo -> 1
Leyendo -> 500
Leyendo -> 20
Leyendo -> 15
Buffer lleno. Ordenando internamente...
Escribiendo chunk_02.tmp... OK.
Buffer ordenado: [1, 15, 20, 500]
Buffer limpiado.

Fase 1 completada. 2 chunks generados.

Iniciando Fase 2: Fusión Externa (K-Way Merge)
K=2. Fusión en progreso...
Fusión completada. Archivo final: output.sorted.txt
```

## Archivos Generados

**chunk_01.tmp, chunk_02.tmp, etc.**
Archivos temporales con datos ordenados parcialmente.

**output.sorted.txt**
Archivo final con todos los datos ordenados de menor a mayor.

Contenido esperado:
```
1
5
15
20
99
105
210
500
```

## Características Implementadas

**POO Avanzado**
- Clase base abstracta DataSource
- Herencia con SerialSource y FileSource
- Polimorfismo con métodos virtuales

**Estructuras de Datos**
- Lista circular doblemente enlazada implementada manualmente
- Gestión de memoria con punteros (new/delete)
- Sin uso de contenedores STL para almacenamiento

**Algoritmos**
- Insertion Sort para ordenar chunks en memoria
- K-Way Merge para fusión de archivos externos

**Comunicación Serial**
- Lectura real de puerto COM/tty usando WinAPI (Windows) o POSIX (Linux)
- Detección automática de puertos disponibles
- Timeout automático cuando no hay más datos

## Requisitos del Caso de Estudio

1. Conexión Serial: Lee del puerto COM/tty del Arduino
2. Buffer Circular: Lista circular de tamaño fijo
3. Generación de Chunks: Archivos .tmp ordenados
4. Fusión Externa: K-Way Merge
5. Ordenamiento Correcto: Archivo final ordenado

6. Exclusividad de Punteros: Sin STL containers
7. POO Avanzado: Clases abstractas y herencia
8. Gestión de Memoria: Destructores virtuales, sin fugas
9. Eficiencia: Memoria constante definida por el buffer
