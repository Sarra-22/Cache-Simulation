#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

struct Request {
    uint32_t addr;
    uint32_t data;
    int we;
};

struct Result {
    size_t cycles ;
    size_t misses ;
    size_t hits ;
    size_t primitiveGateCount ;
};

struct TLBEntry {
    uint32_t virt_block; 
    uint32_t physical_block;
};


#endif