#include "FreeRTOS.h"


extern "C" size_t malloc_cnt;
extern "C" size_t custom_cnt;

void* operator new(size_t size) { 
    malloc_cnt += size;
    return pvPortMalloc(size); 
}

void* operator new(size_t size, uint8_t* ptr) { 
    custom_cnt += size;
    return ptr; 
}

void operator delete(void *ptr) { 
    vPortFree(ptr); 
}

void operator delete(void *ptr, size_t) { 
    vPortFree(ptr); 
}
