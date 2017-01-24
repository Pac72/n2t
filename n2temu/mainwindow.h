#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "emu.h"
#include "codetablemodel.h"
#include "memtablemodel.h"

#include <QMainWindow>
#include <QMutex>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

private slots:
    void on_actionStep_triggered();
    void onRegisterChanged(CPURegister reg, int newValue);
    void onMemoryChanged(int address, int value);
    void onTimeout();
    void initFromSettings();

    void on_actionRun_triggered();

    void on_actionStop_triggered();

    void on_actionReset_triggered();

    void on_horizontalSliderSpeed_valueChanged(int value);

    void on_actionExit_triggered();

    void on_actionOpen_triggered();

private:
    void initializeUi();
    void updateTableViewCode(int newValue);
    void enableUiElements(bool enable);

    bool realtimeNotifications;
    bool running;

    Ui::MainWindow *ui;
    Emu *emu;
    CodeTableModel *codeModel;
    MemTableModel *memModel;
    QTimer *timer;
    int freeRunningCycles;
    mutable QMutex mutex;
};

#endif // MAINWINDOW_H
