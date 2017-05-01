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
    virtual quint16 peek(int addr) const = 0;
    inline int getSize() const { return size; }

protected:
    int size;
};

class RAM16: public AbsMemory
{
public:
    RAM16(int size);
    ~RAM16() { delete[] mem; }

    inline virtual quint16 peek(int addr) const {
        if (addr >= 0 && addr < size) {
            return mem[addr];
        } else {
            return 0;
        }
    }
    inline virtual void poke(int addr, quint16 value) {
        if (addr >= 0 && addr < size) {
            mem[addr] = value;
        }
    }
    void clear();

private:
    quint16 *mem;
};

class ROM16: public AbsMemory
{
public:
    ROM16(int size) : AbsMemory(size) { mem = new quint16[size]; }
    ~ROM16() { delete mem; }

    void initialize(const ROM16Loader &loader);
    inline virtual quint16 peek(int addr) const { return mem[addr]; }
    inline virtual void poke(int addr, quint16 value) { Q_UNUSED(addr); Q_UNUSED(value); }

private:
    quint16 *mem;

};

class VideoFramebuffer: public AbsMemory
{
public:
    VideoFramebuffer(VideoWidget *videoWidget) : AbsMemory(8192) { this->videoWidget = videoWidget; }
    ~VideoFramebuffer() { videoWidget = NULL; }

    inline virtual quint16 peek(int addr) const { return videoWidget->getMemory(addr); }
    inline virtual void poke(int addr, quint16 value) { videoWidget->setMemory(addr, value); }
    void clear();

private:
    VideoWidget *videoWidget;
};

#endif // MEMORY_H
