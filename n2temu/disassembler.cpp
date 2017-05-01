#include "disassembler.h"

#include <QDebug>

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

Disassembler::Disassembler()
{
    hexMode = false;

    dests = new char*[8];
    dests[0] = NULL;
    dests[1] = "M";
    dests[2] = "D";
    dests[3] = "MD";
    dests[4] = "A";
    dests[5] = "AM";
    dests[6] = "AD";
    dests[7] = "AMD";

    jumps = new char*[8];
    jumps[0] = NULL;
    jumps[1] = "JGT";
    jumps[2] = "JEQ";
    jumps[3] = "JGE";
    jumps[4] = "JLT";
    jumps[5] = "JNE";
    jumps[6] = "JLE";
    jumps[7] = "JMP";

    last_opcode = 0x77;
    opcodes = new char*[last_opcode + 1]();
    opcodes[0x00] = "D&A";
    opcodes[0x02] = "D+A";
    opcodes[0x07] = "A-D";
    opcodes[0x0c] = "D";
    opcodes[0x0d] = "!D";
    opcodes[0x0e] = "D-1";
    opcodes[0x0f] = "-D";
    opcodes[0x13] = "D-A";
    opcodes[0x15] = "D|A";
    opcodes[0x1f] = "D+1";
    opcodes[0x2a] = "0";
    opcodes[0x30] = "A";
    opcodes[0x31] = "!A";
    opcodes[0x32] = "A-1";
    opcodes[0x33] = "-A";
    opcodes[0x37] = "A+1";
    opcodes[0x3a] = "-1";
    opcodes[0x3f] = "1";
    opcodes[0x40] = "D&M";
    opcodes[0x42] = "D+M";
    opcodes[0x47] = "M-D";
    opcodes[0x53] = "D-M";
    opcodes[0x55] = "D|M";
    opcodes[0x70] = "M";
    opcodes[0x71] = "!M";
    opcodes[0x72] = "M-1";
    opcodes[0x73] = "-M";
    opcodes[0x77] = "M+1";
}

Disassembler::~Disassembler()
{
    delete [] opcodes;
}

void Disassembler::setHexMode(const bool hexMode)
{
    this->hexMode = hexMode;
}

QString Disassembler::translate_A(const quint16 code) const
{
    int value = (int) code;
    QString asmInstr;
    if (hexMode) {
        asmInstr = QString("@%1").arg(value & 0xffff, 4, 16, QChar('0'));
    } else {
        asmInstr = QString("@%1").arg(value);
    }

    return asmInstr;
}

QString Disassembler::translate_C(const quint16 code) const
{
    if ((code & 0xe000) != 0xe000) {
        QString hexCode = QString::number(code, 16);
        qWarning().nospace() << "Bad C-Instruction code 0x" << hexCode;
        return QString("BAD 0x").append(hexCode);
    }
    uint comp_code = (code >> 6) & 0x007f;

    if (comp_code > last_opcode) {
        QString hexCode = QString::number(comp_code, 16);
        qWarning().nospace() << "Invalid C-Instruction comp_code 0x" << hexCode;
        hexCode = QString::number(code, 16);
        return QString("BAD 0x").append(hexCode);
    }
    const char *opcode = opcodes[comp_code];
    if (NULL == opcode) {
        QString hexCode = QString::number(comp_code, 16);
        qWarning().nospace() << "Unknown C-Instruction comp_code 0x" << hexCode;
        hexCode = QString::number(code, 16);
        return QString("BAD 0x").append(hexCode);
    }
    uint jmp_code = code & 0x0007;
    uint dest_code = (code >> 3) & 0x0007;
    QString asmInstr = QString();
    if (dest_code > 0) {
        asmInstr.append(dests[dest_code]);
        asmInstr.append('=');
    }
    asmInstr.append(opcode);
    if (jmp_code > 0) {
        asmInstr.append(';');
        asmInstr.append(jumps[jmp_code]);
    }

    return asmInstr;
}

QString Disassembler::translate(const quint16 code) const
{
    QString result;

    if ((code & 0x8000) == 0) {
        // A-Instruction
        result = translate_A(code);
    } else {
        // C-Instruction
        result = translate_C(code);
    }

    return result;
}
