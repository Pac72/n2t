#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videowidget.h"
#include "codetablemodel.h"
#include "memtablemodel.h"

#include <QAbstractItemView>
#include <QDialog>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMutexLocker>
#include <QPalette>
#include <QSettings>
#include <QTimer>
#include <QUrl>

#include <cmath>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QCoreApplication::setOrganizationName("Pacsoft");
    QCoreApplication::setOrganizationDomain("pacsoft.org");
    QCoreApplication::setApplicationName("n2temu");

    ui->setupUi(this);

    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &MainWindow::onTimeout);

    VideoFramebuffer *vfb = new VideoFramebuffer(ui->videoWidget);
    codeModel = new CodeTableModel(this);
    emu = new Emu(vfb, codeModel);

    memModel = new MemTableModel(this);
    memModel->setEmu(emu);

    QObject::connect(emu, &Emu::registerChanged, this, &MainWindow::onRegisterChanged);
    QObject::connect(emu, &Emu::memChanged, this, &MainWindow::onMemoryChanged);

    initializeUi();

    freeRunningCycles = 1;
    realtimeNotifications = true;
    running = false;

    initFromSettings();

    emu->reset();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initFromSettings() {
    QString romPath;
    QSettings settings;
    if (settings.value("emu/rom").isValid()) {
       QString romPath = settings.value("emu/rom").toString();
       emu->load(romPath);
       ui->tableViewCode->setModel(NULL);
       ui->tableViewCode->setModel(codeModel);
    }

}

void MainWindow::initializeUi() {
    ui->label_Key->setText("0");
    ui->tableViewCode->verticalHeader()->hide();
    ui->tableViewCode->setModel(codeModel);
    ui->tableViewCode->setColumnWidth(0, 40);
    ui->tableViewCode->setColumnWidth(1, 180);
    ui->tableViewCode->setStyleSheet("QHeaderView::section { background-color:lightGray }");

    ui->tableViewMem->verticalHeader()->hide();
    ui->tableViewMem->setModel(memModel);
    ui->tableViewMem->setColumnWidth(0, 40);
    ui->tableViewMem->setColumnWidth(1, 40);
    ui->tableViewMem->setStyleSheet("QHeaderView::section { background-color:lightGray }");

    QHeaderView *codeVerticalHeader = ui->tableViewCode->verticalHeader();
    codeVerticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    codeVerticalHeader->setDefaultSectionSize(14);

    QHeaderView *memVerticalHeader = ui->tableViewMem->verticalHeader();
    memVerticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    memVerticalHeader->setDefaultSectionSize(14);

    on_horizontalSliderSpeed_valueChanged(ui->horizontalSliderSpeed->value());

    enableUiElements(true);
}


void MainWindow::updateTableViewCode(int newValue) {
    QTableView *tv = ui->tableViewCode;
    tv->setUpdatesEnabled(false);
    codeModel->setCurrentAddress(newValue);
    if (newValue < tv->rowAt(0) || newValue >= tv->rowAt(tv->height())) {
        tv->scrollTo(codeModel->index(newValue, 0), QAbstractItemView::PositionAtTop);
    }
    tv->setUpdatesEnabled(true);
}

void MainWindow::onRegisterChanged(CPURegister reg, int newValue) {
    QString qsValue = QString::number(newValue);
    QString qsValueHex = QString::number(((unsigned)newValue) & 0x0ffff, 16);

    switch (reg) {
    case CPU_REG_A:
        ui->lineEdit_reg_a->setText(qsValue);
        ui->lineEdit_reg_a_hex->setText(qsValueHex);
        break;

    case CPU_REG_D:
        ui->lineEdit_reg_d->setText(qsValue);
        ui->lineEdit_reg_d_hex->setText(qsValueHex);
        break;

    case CPU_REG_PC:
        ui->lineEdit_reg_pc->setText(qsValue);
        ui->lineEdit_reg_pc_hex->setText(qsValueHex);
        updateTableViewCode(newValue);
        break;
    }
}

void MainWindow::onMemoryChanged(int address, int value) {
    QTableView *tv = ui->tableViewMem;
    tv->setUpdatesEnabled(false);
    memModel->setHighlightedAddress(address);
    if (realtimeNotifications) {
        if (address < tv->rowAt(0) || address >= tv->rowAt(tv->height())) {
            tv->scrollTo(memModel->index(address, 0), QAbstractItemView::PositionAtTop);
        }
    }
    tv->setUpdatesEnabled(true);
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::onTimeout() {
    QElapsedTimer timer;
    timer.start();
    emu->run(freeRunningCycles);
    qint64 dt = timer.elapsed();
    qWarning("MainWindow::onTimeout(): dt=%ld ms, freeRunningCycles=%d", dt, freeRunningCycles);
}

void MainWindow::on_actionStep_triggered()
{
    QMutexLocker locker(&mutex);

    emu->run(1);
}

static const int TIMER_PERIOD_MS = 50;

void MainWindow::enableUiElements(bool enable) {
    ui->actionRun->setEnabled(enable);
    ui->actionStep->setEnabled(enable);
    ui->actionStop->setEnabled(!enable);
    ui->lineEdit_reg_a->setEnabled(enable);
    ui->lineEdit_reg_d->setEnabled(enable);
    ui->lineEdit_reg_pc->setEnabled(enable);
    ui->lineEdit_reg_a_hex->setEnabled(enable);
    ui->lineEdit_reg_d_hex->setEnabled(enable);
    ui->lineEdit_reg_pc_hex->setEnabled(enable);
}

void MainWindow::on_actionRun_triggered()
{
    QMutexLocker locker(&mutex);

    timer->start(TIMER_PERIOD_MS);

    enableUiElements(false);

    running = true;

    grabKeyboard();
}

void MainWindow::on_actionStop_triggered()
{
    QMutexLocker locker(&mutex);

    timer->stop();

    enableUiElements(true);

    running = false;

    releaseKeyboard();

    emu->poke(24576, 0);
}

void MainWindow::on_actionReset_triggered()
{
    QMutexLocker locker(&mutex);

    emu->reset();

    ui->label_Key->setText(QString::number(emu->peek(24576)));
}

void MainWindow::on_horizontalSliderSpeed_valueChanged(int value)
{
    freeRunningCycles = pow(10.0, value/100.0);
    int cyclesPerSecond = freeRunningCycles*1000/TIMER_PERIOD_MS;
    ui->label_CLK->setText(QString::number(cyclesPerSecond));
    realtimeNotifications = cyclesPerSecond < 100000;
    emu->enableRealtimeNotifications(realtimeNotifications);
    //qWarning("MainWindow::on_horizontalSliderSpeed_valueChanged(): freeRunningCycles=%d", freeRunningCycles);
}


void MainWindow::on_actionOpen_triggered()
{
    QFileDialog fileDlg(this, "Select .hack ROM file", QDir::currentPath(), "hack (*.hack);; All files (*.*)");
    fileDlg.setFileMode(QFileDialog::ExistingFile);
    if (fileDlg.exec() == QDialog::Accepted) {
        QString filename = fileDlg.selectedFiles().value(0);
        QFile file(filename);
        if (file.exists()) {
            QSettings settings;
            settings.setValue("emu/rom", filename);
            settings.sync();
            emu->load(filename);
            ui->tableViewCode->setModel(NULL);
            ui->tableViewCode->setModel(codeModel);
        } else {
            qWarning("Cannot find file " + filename.toLatin1());
        }
    }
}


static int translate(int rawKey) {
    switch(rawKey) {
        case Qt::Key_Enter:     return K_ENTER;
        case Qt::Key_Backspace: return K_BACKSPACE;
        case Qt::Key_Left:      return K_LEFT;
        case Qt::Key_Up:        return K_UP;
        case Qt::Key_Right:     return K_RIGHT;
        case Qt::Key_Down:      return K_DOWN;
        case Qt::Key_Home:      return K_HOME;
        case Qt::Key_End:       return K_END;
        case Qt::Key_PageUp:    return K_PGUP;
        case Qt::Key_PageDown:  return K_PGDN;
        case Qt::Key_Insert:    return K_INS;
        case Qt::Key_Delete:    return K_DEL;
        case Qt::Key_Escape:    return K_ESC;
        case Qt::Key_F1:        return K_F1;
        case Qt::Key_F2:        return K_F2;
        case Qt::Key_F3:        return K_F3;
        case Qt::Key_F4:        return K_F4;
        case Qt::Key_F5:        return K_F5;
        case Qt::Key_F6:        return K_F6;
        case Qt::Key_F7:        return K_F7;
        case Qt::Key_F8:        return K_F8;
        case Qt::Key_F9:        return K_F9;
        case Qt::Key_F10:       return K_F10;
        case Qt::Key_F11:       return K_F11;
        case Qt::Key_F12:       return K_F12;

        default:
            return rawKey;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (running) {
        int emuKey = translate(event->key());
        emu->onKeyDown(emuKey);
        ui->label_Key->setText(QString::number(emu->peek(24576)));
        qWarning("MainWindow::keyPressEvent: raw 0x%08x key %d", event->key(), emuKey);
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (running) {
        int emuKey = translate(event->key());
        emu->onKeyUp(emuKey);
        ui->label_Key->setText(QString::number(emu->peek(24576)));
        qWarning("MainWindow::keyReleaseEvent: raw 0x%08x key %d", event->key(), emuKey);
    } else {
        QMainWindow::keyReleaseEvent(event);
    }
}
