#include "memory.h"
#include "rom16loader.h"
#include <stdlib.h>

RAM16::RAM16(int size) : AbsMemory(size) {
    mem = new quint16[size];
    clear();
}

void RAM16::clear() {
    memset(mem, 0, size * sizeof(quint16));
}

void ROM16::initialize(const ROM16Loader &loader) {
    int dataSize = loader.getDataSize();

    memcpy(mem, loader.getData(), dataSize * sizeof(quint16));

    /*
     * fill the remaining ROM with 0xe000, which is
     * the assembler instruction
     *   D&A
     * which is one of the NOP instructions
     */
    quint16 *pp = mem + dataSize;
    quint16 *pend = mem + size;
    for ( ; pp < pend; pp++) {
        *pp = 0xe000;
    }
   // memset(mem + dataSize, 0, (size - dataSize) * sizeof(quint16));
}

void VideoFramebuffer::clear()
{
    videoWidget->clear();
}
