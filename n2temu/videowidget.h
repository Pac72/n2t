#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QBrush>
#include <QPainter>
#include <QPen>

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    VideoWidget(QWidget *parent);

    void setMemory(quint16 address, quint16 value);
    quint16 getMemory(quint16 address);

    void clear();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    quint16 memory[0x2000];
    QBrush backgroundBrush;
    QPen penOn;
    QPen penOff;
};

#endif // VIDEOWIDGET_H
