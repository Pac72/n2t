#include "rom16loader.h"

#include <stdlib.h>
#include <QFile>
#include <QTextStream>


void ROM16Loader::load(const QString &fileName)
{
    if (data != NULL) {
        free(data);
        data = NULL;
        size = 0;
    }

    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        int currentSize = 0;
        quint16 *pdata = NULL;
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            if (currentSize >= size) {
                size += 4096;
                data = (quint16 *)realloc(data, size * sizeof(quint16));
                pdata = data + currentSize;
            }
            QString line = in.readLine();
            line = line.trimmed();
            quint16 value = 0;
            for (int ii = 0; ii < 16; ii++) {
                quint16 bit = line.at(ii).digitValue();
                value = (value << 1) | bit;
            }
            *pdata++ = value;
            currentSize++;
        }
        inputFile.close();
        size = currentSize;
        if (size > 0) {
            data = (quint16 *)realloc(data, size * sizeof(quint16));
        } else {
            data = NULL;
        }
    }
}
