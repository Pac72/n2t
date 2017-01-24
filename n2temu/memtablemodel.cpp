#include "memtablemodel.h"
#include "emu.h"

MemTableModel::MemTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void MemTableModel::setEmu(Emu *emu) {
    this->emu = emu;
}

QVariant MemTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
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
    return emu->getSize();
}

int MemTableModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

void MemTableModel::emitDataChanged(int idx) {
    QModelIndex topLeft = index(idx, 0);
    QModelIndex bottomRight = index(idx, 1);
    emit dataChanged(topLeft, bottomRight);
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
                return QString::number(index.row());
            } else {
                return QString::number((qint16)emu->peek(index.row()));
            }
            // not needed break;

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

//bool MemTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    if (data(index, role) != value) {
//        // FIXME: Implement me!
//        emit dataChanged(index, index, QVector<int>() << role);
//        return true;
//    }
//    return false;
//}

//Qt::ItemFlags MemTableModel::flags(const QModelIndex &index) const
//{
////    if (!index.isValid())
////        return Qt::NoItemFlags;

//    return Qt::ItemIsEditable; // FIXME: Implement me!
//}
