# Embedded Template Library (ETL)

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

## Features
* Cross-platform
* Headers only
* No dynamic memory allocation (Except [Vector](include/etl/vector.h),
[LinkedList](include/etl/linked_list.h), and [Map](include/etl/map.h))
* Static [string class](include/etl/string.h)
* Static [function class](include/etl/function.h)
* String view in constexpr context
* JSON parser in constexpr context
* No RTTI
* No virtual functions
* Templated compile time constants
* Some useful [mathematics user-defined literals](include/etl/math.h)
* [Complex number](include/etl/complex.h) with custom size
* Additional [keywords](include/etl/keywords.h)

## Installation
```bash
mkdir build
cmake -B build
make -C build
sudo make install -C build
```
All header files will be installed in /usr/local/include

To uninstall:
```bash
sudo make uninstall -C build
```

## How to use
If it's already installed, you can use it right away. To make sure
if it's correctly installed to include path, you can import the 
library to your cmake project:
```cmake
find_package(etl CONFIG REQUIRED)
target_link_libraries(yourApp etl)
```
Or simply add [etl/include](include) to your project include directories
if it's not installed
```cmake
include_directories(etl/include)
```

## Build test
Prerequisites:
* [Google Test](https://github.com/google/googletest)
```bash
cmake -DBUILD_TESTS=ON -B build
make -C build
```
### Run tests
```bash
./build/test/test_all
```

## Build documentation
Prerequisites:
* [Doxygen](https://github.com/doxygen/doxygen.git)
```bash
cmake -DBUILD_DOCS=ON -B build
make -C build
```
Documentation will be generated in 
[build/docs/html/index.html](build/docs/html/index.html)
