#ifndef EMU_H
#define EMU_H

#include <QtCore/qglobal.h>

#include "cpu.h"
#include "memory.h"
#include "codetablemodel.h"

class CPU;
class CodeTableModel;

enum CPURegister { CPU_REG_A, CPU_REG_D, CPU_REG_PC };

enum KeyCode {
        K_ENTER     = 128,
        K_BACKSPACE = 129,
        K_LEFT      = 130,
        K_UP        = 131,
        K_RIGHT     = 132,
        K_DOWN      = 133,
        K_HOME      = 134,
        K_END       = 135,
        K_PGUP      = 136,
        K_PGDN      = 137,
        K_INS       = 138,
        K_DEL       = 139,
        K_ESC       = 140,
        K_F1        = 141,
        K_F2        = 142,
        K_F3        = 143,
        K_F4        = 144,
        K_F5        = 145,
        K_F6        = 146,
        K_F7        = 147,
        K_F8        = 148,
        K_F9        = 149,
        K_F10       = 150,
        K_F11       = 151,
        K_F12       = 152
};

class Emu: public AbsMemory
{
    Q_OBJECT

public:
    Emu(VideoFramebuffer *videofb, CodeTableModel *codeModel);
    ~Emu();
    void reset();
    bool run(int steps, const bool stopOnBreakpoints = true);
    inline bool breakpointAt(const int addr) const { return breakpoints[addr]; }
    bool toggleBreakpoint(const int addr) { return breakpoints[addr] = !breakpoints[addr]; }
    void clearAllBreakpoints();
    bool load(const QString &romPath);
    void emitRegistersChanged() const ;
    void setClearRAMOnReset(bool clearRAMOnReset);

    virtual quint16 peek(int addr) const;
    virtual void poke(int addr, quint16 value);
    int get_reg_pc() const;

    quint16 fetch(int addr);
    void enableRealtimeNotifications(bool value);

    void onKeyDown(int key);
    void onKeyUp(int key);

signals:
    void registerChanged(CPURegister reg, int newValue) const;
    void memChanged(int address, int newValue) const;

private:
    bool realtimeNotifications;
    int key;

    CPU *cpu;
    ROM16 *rom;
    RAM16 *ram;
    VideoFramebuffer *videofb;
    CodeTableModel *codeModel;
    bool clearRAMOnReset;
    bool *breakpoints;
};

#endif // EMU_H
