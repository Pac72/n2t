#ifndef CODETABLEMODEL_H
#define CODETABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QString>

class CodeTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CodeTableModel(QObject *parent = 0);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setCurrentAddress(int value);
    void load(const QString &fileName);

    void emitDataChanged(int idx);

private:
    QVector<QString> asmLines;
    int currentAddress;
};

#endif // CODETABLEMODEL_H
