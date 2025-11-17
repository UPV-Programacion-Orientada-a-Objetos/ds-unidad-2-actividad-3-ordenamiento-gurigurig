cmake_minimum_required(VERSION 3.0)
project(16Nov)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(16Nov 
    main.cpp
    SerialSource.cpp
    SerialSource.h
    FileSource.cpp
    FileSource.h
    CircularBuffer.cpp
    CircularBuffer.h
    MergeSort.cpp
    MergeSort.h
    DataSource.h
)

target_link_libraries(16Nov Threads::Threads)

if(WIN32)
    target_compile_definitions(16Nov PRIVATE _WIN32_WINNT=0x0601)
endif()