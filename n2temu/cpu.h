#ifndef CPU_H
#define CPU_H

#include <QtCore/qglobal.h>

#include "emu.h"

class Emu;

class CPU
{
public:
    CPU(Emu *emu);
    void reset();
    void execute();
    inline qint16 get_reg_a() { return reg_a; }
    inline qint16 get_reg_d() { return reg_d; }
    inline quint16 get_reg_pc() { return reg_pc; }

private:
    void execute_a_instr(quint16 instr);
    void execute_c_instr(quint16 instr);
    qint16 get_alu_value(int comp);
    void put_alu_value(qint16 value, int dest);
    void update_pc(qint16 value, int jump);
    inline qint16 readMemory();

    Emu *emu;
    qint16 reg_a;
    qint16 reg_d;
    quint16 reg_pc;
};

#endif // CPU_H
