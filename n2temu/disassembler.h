#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <QString>

class Disassembler
{
private:
    char **opcodes;
//    const char *dests[8] = {NULL, "M", "D", "MD", "A", "AM", "AD", "AMD"};
//    const char *jumps[8] = {NULL, "JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP"};
    char **dests;
    char **jumps;
    uint last_opcode;
    bool debugInfoAvailable;
    bool hexMode;

    QString translate_A(const quint16 code) const;
    QString translate_C(const quint16 code) const;
public:
    Disassembler();
    ~Disassembler();
    QString translate(const quint16 code) const;
    void setHexMode(const bool hexMode);
    inline bool isDebugInfoAvailable() { return debugInfoAvailable; }
};

#endif // DISASSEMBLER_H
