#include "memtablemodel.h"
#include "emu.h"

MemTableModel::MemTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    highlightedAddress = -1;
    hexMode = false;
}

void MemTableModel::setEmu(Emu *emu) {
    this->emu = emu;
}

QVariant MemTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);

    if(role == Qt::DisplayRole) {
        if (0 == section) {
            return QString("Addr");
        } else {
            return QString("Data");
        }
    }

    return QVariant();
}


int MemTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return emu->getSize();
}

int MemTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 2;
}

void MemTableModel::emitDataChanged(int idx) {
    QModelIndex topLeft = index(idx, 0);
    QModelIndex bottomRight = index(idx, 1);
    emit dataChanged(topLeft, bottomRight);
}

void MemTableModel::refresh()
{
    highlightedAddress = -1;
    emit dataChanged(index(0, 0), index(emu->getSize(), 1));
}

void MemTableModel::setHighlightedAddress(int newAddress) {
    if (highlightedAddress != newAddress) {
        int previousAddress = highlightedAddress;
        highlightedAddress = newAddress;
        emitDataChanged(previousAddress);
        emitDataChanged(highlightedAddress);
    }
}

QVariant MemTableModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            if (0 == index.column()) {
                if (hexMode) {
                    return QString("%1").arg(index.row(), 4, 16, QChar('0'));
                } else {
                    return QString::number(index.row());
                }
            } else {
                qint16 value = (qint16)emu->peek(index.row());
                if (hexMode) {
                    return QString("%1").arg(value, 4, 16, QChar('0'));
                } else {
                    return QString::number(value);
                }
            }
            break;

        case Qt::TextAlignmentRole:
            if (index.column() == 0) {
                return Qt::AlignRight;
            }
            break;

        case Qt::BackgroundRole:
            if (index.row() == highlightedAddress) {
                QBrush redBackground(Qt::red);
                return redBackground;
            }
            break;

        default:
            break;
    }

    return QVariant();
}

void MemTableModel::setHexMode(bool hexMode) {
    this->hexMode = hexMode;
    emit dataChanged(index(0, 0), index(emu->getSize(), 1));
}

