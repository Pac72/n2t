#include "memory.h"
#include "rom16loader.h"
#include <stdlib.h>

RAM16::RAM16(int size) : AbsMemory(size) {
    mem = new quint16[size];
    memset(mem, 0, size * sizeof(quint16));
}

inline quint16 RAM16::peek(int addr) {
    if (addr >= 0 && addr < size) {
        return mem[addr];
    } else {
        return 0;
    }
}

inline void RAM16::poke(int addr, quint16 value) {
    if (addr >= 0 && addr < size) {
        mem[addr] = value;
    }
}

void ROM16::initialize(const ROM16Loader &loader) {
    if (mem != NULL) {
        delete mem;
    }
    size = loader.getDataSize();
    mem = new quint16[size];
    memcpy(mem, loader.getData(), size * sizeof(quint16));
}
