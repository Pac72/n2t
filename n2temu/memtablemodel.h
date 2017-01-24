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
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Editable:
//    bool setData(const QModelIndex &index, const QVariant &value,
//                 int role = Qt::EditRole) override;

//    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void setHighlightedAddress(int newAddress);
    void emitDataChanged(int idx);

private:
    int highlightedAddress;
    Emu *emu;
};

#endif // DATATABLEMODEL_H
