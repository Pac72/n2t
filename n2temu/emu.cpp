#include "emu.h"

#include "memory.h"
#include "cpu.h"
#include "videowidget.h"
#include "rom16loader.h"
#include "codetablemodel.h"
#include "memtablemodel.h"

#include <QDebug>
#include <QFile>
#include <QStringRef>

Emu::Emu(VideoFramebuffer *videofb,
         CodeTableModel *codeModel) : AbsMemory(65536) {
    this->videofb = videofb;
    this->codeModel = codeModel;
    realtimeNotifications = true;

    codeModel->setEmu(this);

    ram = new RAM16(16384);

    rom = new ROM16(32768);

    cpu = new CPU(this);

    breakpoints = new bool[32768];

    clearAllBreakpoints();

    key = 0;
}

Emu::~Emu() {
    if (ram != NULL) {
        delete ram;
        ram = NULL;
    }
    if (rom != NULL) {
        delete rom;
        rom = NULL;
    }
    if (videofb != NULL) {
        delete videofb;
        videofb = NULL;
    }

    delete [] breakpoints;
}

bool Emu::load(const QString &romPath) {
    bool loadedSuccesfully = false;

    ROM16Loader loader;
    if (loader.load(romPath)) {
        rom->initialize(loader);

        int ii = romPath.lastIndexOf('.');
        QStringRef romBasePath(&romPath, 0, ii);

        QString romHackdbgPath = romBasePath.toString() + ".hackdbg";
        codeModel->setROM(rom);
        QFile romHackdbgFile(romHackdbgPath);

        codeModel->loadDebugInfo(romHackdbgFile);

        loadedSuccesfully = true;
    }

    return loadedSuccesfully;
}

void Emu::enableRealtimeNotifications(bool value) {
    realtimeNotifications = value;
}

quint16 Emu::peek(int addr) const {
    if (addr < 0) {
        // invalid address
        return 0;
    }
    if (addr < 16384) {
        return ram->peek(addr);
    } else if (addr < 24576) {
        return videofb->peek(addr);
    } else if (24576 == addr) {
        // keyboard
        return key;
    } else {
        // invalid address
        return 0;
    }
}

void Emu::poke(int addr, quint16 value) {
    if (addr < 0) {
        // invalid address
        qWarning("Emu::poke(): invalid negative address %d", addr);
        return;
    }

    bool changed = false;
    if (addr < 16384) {
        if (ram->peek(addr) != value) {
            changed = true;
            ram->poke(addr, value);
        }
    } else if (addr < 24576) {
        if (videofb->peek(addr) != value) {
            changed = true;
            videofb->poke(addr, value);
        }
    } else if (24576 == addr) {
        // useful to reset keyboard
        key = value;
    } else {
        // invalid address
        qWarning("Emu::poke(): invalid address 0x%04x", addr);
        return;
    }
    if (realtimeNotifications) {
        if (changed) {
            emit memChanged(addr, value);
        }
    }
}

quint16 Emu::fetch(int addr) {
    return rom->peek(addr);
}

void Emu::reset() {
    cpu->reset();
    if (clearRAMOnReset) {
        ram->clear();
        videofb->clear();
    }

    emit registerChanged(CPU_REG_A, cpu->get_reg_a());
    emit registerChanged(CPU_REG_D, cpu->get_reg_d());
    emit registerChanged(CPU_REG_PC, cpu->get_reg_pc());

    this->key = 0;
}

void Emu::emitRegistersChanged() const {
    emit registerChanged(CPU_REG_A, cpu->get_reg_a());
    emit registerChanged(CPU_REG_D, cpu->get_reg_d());
    emit registerChanged(CPU_REG_PC, cpu->get_reg_pc());
}

void Emu::setClearRAMOnReset(bool clearRAMOnReset)
{
    this->clearRAMOnReset = clearRAMOnReset;
}

int Emu::get_reg_pc() const
{
    return cpu->get_reg_pc();
}

bool Emu::run(int steps, const bool stopOnBreakpoints) {
    bool stoppedOnBreakpoint = false;
    qint16 reg_a = cpu->get_reg_a();
    qint16 reg_d = cpu->get_reg_d();
    quint16 reg_pc = cpu->get_reg_pc();

    for (int ii = 0; ii < steps; ii++) {
        if (cpu->execute(stopOnBreakpoints)) {
            stoppedOnBreakpoint = true;
            break;
        }
    }

    if (reg_a != cpu->get_reg_a()) {
        emit registerChanged(CPU_REG_A, cpu->get_reg_a());
    }
    if (reg_d != cpu->get_reg_d()) {
        emit registerChanged(CPU_REG_D, cpu->get_reg_d());
    }
    if (reg_pc != cpu->get_reg_pc()) {
        emit registerChanged(CPU_REG_PC, cpu->get_reg_pc());
    }

    return stoppedOnBreakpoint;
}

void Emu::clearAllBreakpoints()
{
    for (int ii = 0; ii < 32768; ii++) {
        breakpoints[ii] = false;
    }
}

void Emu::onKeyDown(int key) {
    this->key += key;
}

void Emu::onKeyUp(int key) {
    this->key -= key;
}
