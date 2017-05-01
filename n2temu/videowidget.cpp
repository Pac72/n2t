#include "videowidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <QPen>

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent) {
    backgroundBrush = QBrush(Qt::white);
    penOn = QPen(Qt::black);
    penOn.setWidth(1);
    penOff = QPen(Qt::white);
    penOff.setWidth(1);
    setAutoFillBackground(false);

    memset(memory, 0, 0x2000 * sizeof(quint16));
}

void VideoWidget::clear() {
    memset(memory, 0, 0x2000 * sizeof(quint16));
    update(0, 0, 512, 256);
}

void VideoWidget::setMemory(quint16 address, quint16 value) {
    quint16 addr = address & 0x1fff;
    if (memory[addr] != value) {
        int xx0 = (addr & 0x1f) << 4;
        int yy0 = addr >> 5;
        memory[addr] = value;
        //qWarning("VideoWidget::setMemory(): calling update(%d,%d,%d,%d)", xx0, yy0, 16, 1);
        update(xx0, yy0, 16, 1);
    }
}

quint16 VideoWidget::getMemory(quint16 address) {
    return memory[address & 0x1fff];
}

void VideoWidget::paintEvent(QPaintEvent *event) {
    QPainter painter;
    painter.begin(this);
    painter.setBrush(backgroundBrush);

    //qWarning("VideoWidget::paintEvent(): event->rect()=(%d,%d,%d,%d)", event->rect().left(), event->rect().top(), event->rect().right(), event->rect().bottom());
    int yy0 = event->rect().top();
    int yy1 = event->rect().bottom();
    int xx, yy, addr;
    int ax0 = event->rect().left() >> 4;
    int xx0 = ax0 << 4;
    int ax1 = event->rect().right() >> 4;
    int xx1;

    //qWarning("VideoWidget::paintEvent(): yy0=%d yy1=%d xx0=%d ax0=%d ax1=%d", yy0, yy1, xx0, ax0, ax1);

    for (yy = yy0; yy <= yy1; yy++) {
        int addry0 = yy << 5;
        int addr0 = addry0 + ax0;
        int addr1 = addry0 + ax1;
        xx = xx0;
        for (addr = addr0; addr <= addr1; addr++) {
            quint16 ww = memory[addr];
            xx1 = xx + 15;
            for (; xx <= xx1; xx++) {
                bool pixel_on = (ww & 1) != 0;
                ww >>= 1;
                if (pixel_on) {
                    painter.setPen(penOn);
                } else {
                    painter.setPen(penOff);
                }
                painter.drawPoint(xx, yy);
            }
        }
    }
}
