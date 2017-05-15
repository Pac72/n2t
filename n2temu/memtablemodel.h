#ifndef DATATABLEMODEL_H
#define DATATABLEMODEL_H

#include "emu.h"

#include <QAbstractTableModel>

class MemTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MemTableModel(QObject *parent = 0);

    void setEmu(Emu *emu);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    // Editable:
//    bool setData(const QModelIndex &index, const QVariant &value,
//                 int role = Qt::EditRole);

//    Qt::ItemFlags flags(const QModelIndex& index) const;

    void setHighlightedAddress(int newAddress);
    void emitDataChanged(int idx);
    void refresh();
    void setHexMode(const bool hexMode);

private:
    int highlightedAddress;
    Emu *emu;
    bool hexMode;
};

#endif // DATATABLEMODEL_H
