# Embedded Template Library (ETL) using FreeRTOS v2 for STM32 projects

## Overview
C++ is a great language to use in embedded applications. 
It has features that help our code to maximize compile
time computation and better readability.

The C++ standard template library (STL) has well tested 
functionality. but there are some parts that do not fit 
well with limited resource requirements needed by embedded
system. Also, most embedded C++ compilers do not support 
the standard beyond C++03. 

ETL is not meant to completely replace STL, but to complement
it. ETL also has some useful components that are not present
in STL.

## Requirements
* C++17
* cmake minimum version 3.7
* STM32CubeMx generated code

## Features
* Static [string class](include/etl/string.h)
* Static [function class](include/etl/function.h)
* No RTTI
* No virtual functions
* Templated compile time constants
* Some useful [mathematics user-defined literals](include/etl/math.h)
* [Complex number](include/etl/complex.h) with custom size
* Additional [keywords](include/etl/keywords.h)
* FreeRTOS [thread](include/etl/thread.h), [timer](include/etl/timer.h), 
[semaphore](include/etl/semaphore.h), [mutex](include/etl/mutex.h), 
[queue](include/etl/queue.h), [event](include/etl/event.h)
* FreeRTOS [heap management](include/etl/heap.h)
* FreeRTOS [dynamic allocation](src/dynamic_allocation.cpp)

## How to use
* Clone this branch to your STM32 project folder. For example:
```bash
git clone -b FreeRTOS https://github.com/aufam/etl.git your_project_path/Middlewares/Third_Party/etl
```
* Add these line to your project CMakeLists.txt:
```cmake
add_subdirectory(Middlewares/Third_Party/etl)
target_link_libraries(${PROJECT_NAME}.elf etl)
```
* (Optional) Add the submodule:
```bash
git submodule add -b FreeRTOS https://github.com/aufam/etl.git your_project_path/Middlewares/Third_Party/etl
```
