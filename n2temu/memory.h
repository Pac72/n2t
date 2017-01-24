#ifndef MEMORY_H
#define MEMORY_H

#include "videowidget.h"
#include "rom16loader.h"

#include <QtCore/qglobal.h>
#include <QObject>

//template <typename TAddr, typename TValue> class AbsMemory<TAddr, TValue>




class AbsMemory: public QObject
{
public:
    AbsMemory(int size) { this->size = size; }

    virtual void poke(int addr, quint16 value) = 0;
    virtual quint16 peek(int addr) = 0;
    inline int getSize() { return size; }

protected:
    int size;
};

class RAM16: AbsMemory
{
public:
    RAM16(int size);
    ~RAM16() { delete[] mem; }

    inline virtual quint16 peek(int addr);
    inline virtual void poke(int addr, quint16 value);

private:
    quint16 *mem;
};

class ROM16: AbsMemory
{
public:
    ROM16() : AbsMemory(0) { mem = NULL; }
    ~ROM16() { delete mem; }

    void initialize(const ROM16Loader &loader);
    inline virtual quint16 peek(int addr) { return mem[addr]; }
    inline virtual void poke(int addr, quint16 value) { }

private:
    quint16 *mem;

};

class VideoFramebuffer: AbsMemory
{
public:
    VideoFramebuffer(VideoWidget *videoWidget) : AbsMemory(8192) { this->videoWidget = videoWidget; }
    ~VideoFramebuffer() { videoWidget = NULL; }

    inline virtual quint16 peek(int addr) { return videoWidget->getMemory(addr); }
    inline virtual void poke(int addr, quint16 value) { videoWidget->setMemory(addr, value); }
private:
    VideoWidget *videoWidget;
};

#endif // MEMORY_H
