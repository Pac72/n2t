#include "codetablemodel.h"
#include "disassembler.h"

#include <QBrush>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStringList>

enum Columns { ADDR_COL = 0, LABEL_COL = 1, CODE_COL = 2 };

CodeTableModel::CodeTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    currentAddress = -1;
    disassembler = new Disassembler();
    debugInfoAvailable = false;
    hexMode = false;
    rom = NULL;

    backgroundColor = new QBrush[4];
    backgroundColor[0] = Qt::white;
    backgroundColor[1] = Qt::yellow;
    backgroundColor[2] = Qt::red;
    backgroundColor[3] = Qt::darkYellow;
}

CodeTableModel::~CodeTableModel()
{
    delete [] backgroundColor;
    delete disassembler;
}

QVariant CodeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);

    if(role == Qt::DisplayRole) {
        switch(section) {
        case ADDR_COL:
            return QString("Addr");
            break;

        case LABEL_COL:
            return QString("Label");
            break;

        case CODE_COL:
            if (showSrc) {
                return QString("Source");
            } else {
                return QString("Asm");
            }
            break;

        default:
            break;
        }
    }

    return QVariant();
}

int CodeTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return rom != NULL ? rom->getSize() : 0;
}

int CodeTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 3;
}

QVariant CodeTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                case ADDR_COL:
                    if (hexMode) {
                        return QString("%1").arg(index.row(), 4, 16, QChar('0'));
                    } else {
                        return QString("%1").number(index.row());
                    }

                case LABEL_COL:
                    if (showSrc) {
                        return labels[index.row()];
                    }
                    break;

                case CODE_COL:
                {
                    if (showSrc) {
                        if (hexMode) {
                            return asmLinesHex[index.row()];
                        } else {
                            return asmLines[index.row()];
                        }
                    } else {
                        quint16 code = rom->peek(index.row());
                        QString asmInstr = disassembler->translate(code);
                        return asmInstr;
                    }
                }
            }
            break;

        case Qt::TextAlignmentRole:
            if (index.column() == 0) {
                return Qt::AlignRight;
            }
            break;

        case Qt::ToolTipRole:

            break;

        case Qt::BackgroundRole:
            {
                bool isCurrentAddress = index.row() == currentAddress;
                bool isBreakpointAddress = emu->breakpointAt(index.row());
                int colorIdx = 0;
                if (isCurrentAddress) {
                    colorIdx |= 1;
                }
                if (isBreakpointAddress) {
                    colorIdx |= 2;
                }
                return backgroundColor[colorIdx];
            }
            break;

        default:
            break;
    }

    return QVariant();
}

void CodeTableModel::emitDataChanged(int idx)
{
    QModelIndex topLeft = index(idx, 0);
    QModelIndex bottomRight = index(idx, 2);
    emit dataChanged(topLeft, bottomRight);
}

void CodeTableModel::setCurrentAddress(int value)
{
    if (currentAddress != value) {
        int previousAddress = currentAddress;
        currentAddress = value;
        emitDataChanged(previousAddress);
        emitDataChanged(currentAddress);
    }
}

void CodeTableModel::setROM(const ROM16 *rom)
{
    this->rom = rom;
    currentAddress = 0;
    int romSize = rom->getSize();
    emit dataChanged(index(0, 0), index(romSize, 2));
}

void CodeTableModel::setShowSource(bool showSrc)
{
    this->showSrc = showSrc;
    emit headerDataChanged(Qt::Horizontal, 2, 2);
    emit dataChanged(index(0, 0), index(rom->getSize(), 2));
}

void CodeTableModel::setHexMode(bool hexMode) {
    this->hexMode = hexMode;
    this->disassembler->setHexMode(hexMode);
    emit dataChanged(index(0, 0), index(rom->getSize(), 2));
}

const char CODE_HEADER = 'C';
const char LABEL_HEADER = 'L';
const QChar AT_QChar = QChar('@');

void CodeTableModel::loadDebugInfo(QFile &inputFile)
{
    debugInfoAvailable = false;
    asmLines.clear();
    asmLines.resize(rom->getSize());
    asmLinesHex.clear();
    asmLinesHex.resize(rom->getSize());
    labels.clear();
    labels.resize(rom->getSize());

    if (!inputFile.exists()) {
        qInfo().nospace() << "Debug file " << inputFile.fileName() << " not available";
        return;
    }

    if (inputFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            line = line.trimmed();
            if (line.length() < 1) {
                continue;
            }

            QStringList parts = line.split('\t');
            int address;
            QString asmLine;
            QString asmLineHex;
            QString label;

            switch(line.at(0).toLatin1()) {
                case CODE_HEADER:
                    address = parts[2].toInt();
                    asmLine = parts.last();
                    asmLines[address] = asmLine;
                    asmLineHex = asmLine;
                    if (AT_QChar == asmLine[0]) {
                        if (asmLine[1].isDigit()) {
                            int value = asmLine.mid(1).toInt();
                            asmLineHex = QString("@%1").arg(value & 0xffff, 4, 16, QChar('0'));
                        }
                    }
                    asmLinesHex[address] = asmLineHex;
                    break;

                case LABEL_HEADER:
                    address = parts[2].toInt();
                    label = parts[1];
                    labels[address] = label;
                    break;

                default:
                    break;
            }

        }
        inputFile.close();
        qInfo().nospace() << "Debug infos loaded from hackdbg file " << inputFile.fileName();
        debugInfoAvailable = true;
        emit dataChanged(index(0, 0), index(asmLines.length(), 2));
    } else {
        qWarning().nospace() << "Cannot not open hackdbg file " << inputFile.fileName();
    }
}
