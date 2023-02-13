# Embedded Template Library (ETL)

## Overview
C++ is a great language to use in embedded applications. 
It has features that help our code to maximize compile
time computation and better readability.

The C++ standard template library (STL) has well tested 
functionality. but there are some parts that do not fit 
well with limited resource requirements. Also, most 
embedded C++ compilers do not support the standard beyond 
C++03. 

ETL is not meant to completely replace STL, but to complement
it. ETL also has some useful components that are not present
in STL.

## Requirement
* C++17
* math library 

## Features
* Cross-platform
* Headers only
* No dynamic memory allocation (Except [Vector](include/etl/vector.h),
[LinkedList](include/etl/linked_list.h), and [Map](include/etl/map.h))
* No RTTI
* No virtual functions
* Templated compile time constants
* Many utilities for template support
* Python-like [keywords](include/etl/python_keywords.h)
* Some useful [mathematics user-defined literals](include/etl/math.h)
* [Complex number](include/etl/complex.h) with custom size
* Easy to read and documented source

## How to use
Simply add [include](include) to your project include directories

## Build test and documentation
Prerequisites:
* Google Test
* Doxygen

```bash
mkdir build
cmake -B build
make -C build
```
Documentation will be generated in 
[build/docs/html/index.html](build/docs/html/index.html)

## Run test
```bash
./build/test
```
