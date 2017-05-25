#ifndef ROM16LOADER_H
#define ROM16LOADER_H

#include <QtCore/qglobal.h>
#include <QString>

class ROM16Loader
{
public:
    ROM16Loader() { data = NULL; size = 0; }
    bool load(const QString &filename);
    inline quint16 *getData() const { return data; }
    inline int getDataSize() const { return size; }

private:
    quint16 *data;
    int size;
};

#endif // ROM16LOADER_H
