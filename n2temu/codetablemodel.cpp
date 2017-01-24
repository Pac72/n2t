#include "codetablemodel.h"

#include <QBrush>
#include <QFile>
#include <QTextStream>
#include <QStringList>

CodeTableModel::CodeTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    currentAddress = 0;
}

QVariant CodeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole) {
        if (0 == section) {
            return QString("Addr");
        } else {
            return QString("Code");
        }
    }

    return QVariant();
}

int CodeTableModel::rowCount(const QModelIndex &parent) const
{
    return asmLines.length();
}

int CodeTableModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant CodeTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole:
            if (0 == index.column()) {
                return QString::number(index.row());
            } else {
                return asmLines.at(index.row());
            }
            // not needed break;

        case Qt::BackgroundRole:
            if (index.row() == currentAddress) {
                QBrush redBackground(Qt::red);
                return redBackground;
            }
            break;

        default:
            break;
    }

    return QVariant();
}

void CodeTableModel::emitDataChanged(int idx) {
    QModelIndex topLeft = index(idx, 0);
    QModelIndex bottomRight = index(idx, 1);
    emit dataChanged(topLeft, bottomRight);
}

void CodeTableModel::setCurrentAddress(int value) {
    if (currentAddress != value) {
        int previousAddress = currentAddress;
        currentAddress = value;
        emitDataChanged(previousAddress);
        emitDataChanged(currentAddress);
    }
}

static const QChar CODE_HEADER('C');

void CodeTableModel::load(const QString &fileName)
{
    QFile inputFile(fileName);

    if (inputFile.open(QIODevice::ReadOnly)) {
        asmLines.clear();
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            line = line.trimmed();
            if (line.length() < 1 || line.at(0) != CODE_HEADER) {
                continue;
            }
            QStringList parts = line.split('\t');
            QString asmLine = parts.last();
            asmLines.append(asmLine);
        }
        inputFile.close();
        emit dataChanged(index(0, 0), index(asmLines.length(), 1));
    }
}
