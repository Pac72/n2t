#ifndef CODETABLEMODEL_H
#define CODETABLEMODEL_H

#include "disassembler.h"
#include "emu.h"
#include "memory.h"

#include <QAbstractTableModel>
#include <QBrush>
#include <QFile>
#include <QString>
#include <QVector>

class Emu;

class CodeTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CodeTableModel(QObject *parent = 0);
    ~CodeTableModel();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void setCurrentAddress(int value);
    void setROM(const ROM16 *rom);
    void loadDebugInfo(QFile &file);
    inline bool isDebugInfoAvailable() const { return debugInfoAvailable; }
    void setShowSource(bool showSrc);

    void emitDataChanged(int idx);
    void setHexMode(bool hexMode);

    void setEmu(Emu *emu) { this->emu = emu; }
private:
    QVector<QString> asmLines;
    QVector<QString> asmLinesHex;
    QVector<QString> labels;
    int currentAddress;
    Disassembler *disassembler;
    Emu *emu;
    const ROM16 *rom;
    bool showSrc;
    bool debugInfoAvailable;
    bool hexMode;
    QBrush *backgroundColor;
};

#endif // CODETABLEMODEL_H
