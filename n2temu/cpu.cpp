#include "cpu.h"

#include <QtCore/qglobal.h>

static const quint16 A_INSTRMASK = 0x8000u;

static const quint16 IA_VALUEMASK = 0x7fffu;

static const quint16 IC_COMPMASK = 0x1fc0u;
static const quint16 IC_DESTMASK = 0x0038u;
static const quint16 IC_JUMPMASK = 0x0007u;
static const int IC_COMPSHIFT = 6;
static const int IC_DESTSHIFT = 3;

static const int IC_DEST_M_MASK = 0x01;
static const int IC_DEST_D_MASK = 0x02;
static const int IC_DEST_A_MASK = 0x04;

CPU::CPU(Emu *emu)
{
    this->emu = emu;

    reset();
}

void CPU::reset() {
    reg_a = 0;
    reg_d = 0;
    reg_pc = 0;
}

void CPU::execute_a_instr(quint16 instr) {
    quint16 value = instr & IA_VALUEMASK;
    reg_a = value;
    reg_pc++;
}

inline qint16 CPU::readMemory() {
    return (qint16)(emu->peek(reg_a));
}

qint16 CPU::get_alu_value(int comp) {
    switch(comp) {
        case 0x00: //0000000 D&A
            return reg_d & reg_a;
        case 0x02: //0000010 D+A
            return reg_d + reg_a;
        case 0x07: //0000111 A-D
            return reg_a - reg_d;
        case 0x0c: //0001100 D
            return reg_d;
        case 0x0d: //0001101 !D
            return ~reg_d;
        case 0x0e: //0001110 D-1
            return reg_d - 1;
        case 0x0f: //0001111 -D
            return -reg_d;
        case 0x13: //0010011 D-A
            return reg_d - reg_a;
        case 0x15: //0010101 D|A
            return reg_d | reg_a;
        case 0x1f: //0011111 D+1
            return reg_d + 1;
        case 0x2a: //0101010 0
            return 0;
        case 0x30: //0110000 A
            return reg_a;
        case 0x31: //0110001 !A
            return ~reg_a;
        case 0x32: //0110010 A-1
            return reg_a - 1;
        case 0x33: //0110011 -A
            return -reg_a;
        case 0x37: //0110111 A+1
            return reg_a + 1;
        case 0x3a: //0111010 -1
            return -1;
        case 0x3f: //0111111 1
            return 1;
        case 0x40: //1000000 D&M
            return reg_d & readMemory();
        case 0x42: //1000010 D+M
            return reg_d + readMemory();
        case 0x47: //1000111 M-D
            return readMemory() - reg_d;
        case 0x53: //1010011 D-M
            return reg_d - readMemory();
        case 0x55: //1010101 D|M
            return reg_d | readMemory();
        case 0x70: //1110000 M
            return readMemory();
        case 0x71: //1110001 !M
            return ~readMemory();
        case 0x72: //1110010 M-1
            return readMemory() - 1;
        case 0x73: //1110011 -M
            return -readMemory();
        case 0x77: //1110111 M+1
            return readMemory() + 1;
    }
    // we should never reach here
    qWarning("CPU::get_alu_value(): invalid comp 0x%02x", comp);
    return 0;
}

void CPU::put_alu_value(qint16 value, int dest) {
    if ((dest & IC_DEST_M_MASK) != 0) {
        emu->poke(reg_a, (quint16)value);
    }

    if ((dest & IC_DEST_D_MASK) != 0) {
        reg_d = value;
    }

    if ((dest & IC_DEST_A_MASK) != 0) {
        reg_a = value;
    }
}

void CPU::update_pc(qint16 value, int jump) {
    /*
     * default is to go to the next instruction
     */
    reg_pc++;

    switch(jump) {
        case 0x00:
            break;

        case 0x01:
            if (value > 0) {
                reg_pc = reg_a;
            }
            break;

        case 0x02:
            if (0 == value) {
                reg_pc = reg_a;
            }
            break;

        case 0x03:
            if (value >= 0) {
                reg_pc = reg_a;
            }
            break;

        case 0x04:
            if (value < 0) {
                reg_pc = reg_a;
            }
            break;

        case 0x05:
            if (value != 0) {
                reg_pc = reg_a;
            }
            break;

        case 0x06:
            if (value <= 0) {
                reg_pc = reg_a;
            }
            break;

        case 0x07:
            reg_pc = reg_a;
            break;

        default:
            // uh oh...
            qWarning("CPU::update_pc(): invalid jump 0x%02x", jump);
    }
}

void CPU::execute_c_instr(quint16 instr) {
    int comp = (instr & IC_COMPMASK) >> IC_COMPSHIFT;
    int dest = (instr & IC_DESTMASK) >> IC_DESTSHIFT;
    int jump = instr & IC_JUMPMASK;

    qint16 alu_value = get_alu_value(comp);

    put_alu_value(alu_value, dest);

    update_pc(alu_value, jump);
}

bool CPU::execute(const bool stopOnBreakpoints) {
    if (stopOnBreakpoints && emu->breakpointAt(reg_pc)) {
        return true;
    }
    quint16 instr = emu->fetch(reg_pc);

//    qWarning("CPU::execute(): pc=0x%04x i=0x%04x a=0x%04x (%6d) d=0x%04x (%6d)",
//             reg_pc, instr,
//             ((unsigned)reg_a) & 0x0ffff, reg_a,
//             ((unsigned)reg_d) & 0x0ffff, reg_d);

    if ((instr & A_INSTRMASK) == 0) {
        execute_a_instr(instr);
    } else {
        execute_c_instr(instr);
    }

    return false;
}
