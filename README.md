# Embedded Template Library (ETL)

## Overview
C++ is a great language to use in embedded applications. 
It has features that help our code to maximize compile
time computation and better readability.

The C++ standard template library (STL) has well tested 
functionality. but there are some parts that do not fit 
well with limited resource requirements needed by embedded
systems. Also, some embedded compilers do not support 
the standard beyond C++17. 

ETL is not meant to completely replace STL, but to complement
it. ETL also has some useful components that are not present
in STL.

## Requirements
* C++17 minimum
* cmake minimum version 3.14

## Features
* Cross-platform
* Headers only
* No dynamic memory allocation (Except [Vector](include/etl/vector.h),
[LinkedList](include/etl/linked_list.h), and [Map](include/etl/map.h))
* Static [string class](include/etl/string.h)
* Static [function class](include/etl/function.h)
* Return error by value using [Result](include/etl/result.h)
* String view in constexpr context
* JSON parser in constexpr context
* No RTTI
* No virtual functions
* Templated compile time constants
* Some useful [mathematics user-defined literals](include/etl/math.h)
* Additional [keywords](include/etl/keywords.h)

## Installation
```bash
cmake -B build
cmake --build build
sudo cmake --build build --target install
```

To uninstall:
```bash
sudo cmake --build build --target uninstall
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

## Tests
Build tests:
```bash
cmake -DETL_BUILD_TESTS=ON -B build
cmake --build build
```
Run tests:
```bash
ctest --test-dir build/tests --output-on-failure
```

## Documentation
Prerequisites:
* [Doxygen](https://github.com/doxygen/doxygen.git)

Build docs:
```bash
cmake -DETL_BUILD_DOCS=ON -B build
cmake --build build --target docs
```
