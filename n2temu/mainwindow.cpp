#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videowidget.h"
#include "codetablemodel.h"
#include "memtablemodel.h"
#include "emu.h"

#include <QAbstractItemView>
#include <QDialog>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFontDatabase>
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

    freeRunningCycles = 1;
    realtimeNotifications = true;
    running = false;
    hexMode = false;
    clearRAMOnReset = true;

    initializeUi();

    QStringList args = qApp->arguments();

    if (args.size() > 1) {
        QString filename = args[1];
        updateSettingsAndLoadROM(filename);
    } else {
        initFromSettings();
    }

    emu->reset();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateSettingsAndLoadROM(const QString &filename) {
    QFile file(filename);
    if (file.exists()) {
        QFileInfo fileInfo(file);
        QSettings settings;
        settings.setValue("emu/rom", filename);
        lastROMDir = fileInfo.absolutePath();
        settings.setValue("emu/lastROMDir", lastROMDir);
        settings.sync();
        loadROM(filename);
    } else {
        QByteArray filenameba = filename.toLocal8Bit();
        const char *c_filename = filenameba.data();
        qWarning("Cannot find file %s", c_filename);
    }
}

void MainWindow::loadROM(const QString &romPath) {
    if (emu->load(romPath)) {
        if (codeModel->isDebugInfoAvailable()) {
            emit ui->checkBox_show_source_code->setChecked(true);
            codeModel->setShowSource(true);
        } else {
            codeModel->setShowSource(false);
        }
        QString title("n2temu - ");
        title.append(romPath);
        setWindowTitle(title);

        emit ui->checkBox_show_source_code->setEnabled(codeModel->isDebugInfoAvailable());

        emu->clearAllBreakpoints();
        emu->reset();
    }
}

void MainWindow::initFromSettings() {
    QSettings settings;
    if (settings.value("emu/lastROMDir").isValid()) {
        lastROMDir = settings.value("emu/lastROMDir").toString();
    }
    if (settings.value("emu/rom").isValid()) {
       QString romPath = settings.value("emu/rom").toString();
       loadROM(romPath);
    }

}

void MainWindow::initializeUi() {

    QObject::connect(emu, &Emu::registerChanged, this, &MainWindow::onRegisterChanged);
    QObject::connect(emu, &Emu::memChanged, this, &MainWindow::onMemoryChanged);

    QObject::connect(ui->checkBox_show_source_code, SIGNAL(toggled(bool)), ui->actionShowSrc, SLOT(setChecked(bool)));
    QObject::connect(ui->actionShowSrc, SIGNAL(toggled(bool)), ui->checkBox_show_source_code, SLOT(setChecked(bool)));

    QObject::connect(ui->checkBox_hex_mode, SIGNAL(toggled(bool)), ui->actionHexMode, SLOT(setChecked(bool)));
    QObject::connect(ui->actionHexMode, SIGNAL(toggled(bool)), ui->checkBox_hex_mode, SLOT(setChecked(bool)));

    QObject::connect(ui->checkBox_clear_RAM_on_reset, SIGNAL(toggled(bool)), ui->actionResetClearsRAM, SLOT(setChecked(bool)));
    QObject::connect(ui->actionResetClearsRAM, SIGNAL(toggled(bool)), ui->checkBox_clear_RAM_on_reset, SLOT(setChecked(bool)));

    //const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    ui->label_Key->setText("0");

    //ui->tableViewCode->setFont(fixedFont);
    ui->tableViewCode->verticalHeader()->hide();
    ui->tableViewCode->setModel(codeModel);
    ui->tableViewCode->setColumnWidth(0, 40);
    ui->tableViewCode->setColumnWidth(1, 60);
    ui->tableViewCode->setColumnWidth(2, 140);
    ui->tableViewCode->horizontalHeader()->setStretchLastSection(true);

    //ui->tableViewCode->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableViewCode->setStyleSheet("QHeaderView::section { background-color:lightGray }");
    QHeaderView *codeVerticalHeader = ui->tableViewCode->verticalHeader();
    codeVerticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    codeVerticalHeader->setDefaultSectionSize(14);

    //ui->tableViewMem->setFont(fixedFont);
    ui->tableViewMem->verticalHeader()->hide();
    ui->tableViewMem->setModel(memModel);
    ui->tableViewMem->setColumnWidth(0, 40);
    ui->tableViewMem->setColumnWidth(1, 40);

    //ui->tableViewMem->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableViewMem->setStyleSheet("QHeaderView::section { background-color:lightGray }");

    QHeaderView *memVerticalHeader = ui->tableViewMem->verticalHeader();
    memVerticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    memVerticalHeader->setDefaultSectionSize(14);

    on_horizontalSliderSpeed_valueChanged(ui->horizontalSliderSpeed->value());

    emit ui->checkBox_show_source_code->setEnabled(false);
    emit ui->checkBox_clear_RAM_on_reset->setChecked(clearRAMOnReset);

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
    QString qsValue;
    if (hexMode) {
//        qsValue = QString::number(((unsigned)newValue) & 0x0ffff, 16);
        qsValue = QString("%1").arg(((unsigned)newValue) & 0x0ffff, 4, 16, QChar('0'));
    } else {
        qsValue = QString::number(newValue);
    }

    switch (reg) {
    case CPU_REG_A:
        ui->lineEdit_reg_a->setText(qsValue);
        break;

    case CPU_REG_D:
        ui->lineEdit_reg_d->setText(qsValue);
        break;

    case CPU_REG_PC:
        ui->lineEdit_reg_pc->setText(qsValue);
        updateTableViewCode(newValue);
        break;
    }
}

void MainWindow::onMemoryChanged(int address, int value) {
    Q_UNUSED(value);

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
    if (emu->run(freeRunningCycles)) {
        int addr = emu->get_reg_pc();
        qInfo("MainWindow::onTimeout(): breakpoint hit at address %d (0x%.4x)", addr, addr);

        doStop();

        return;
    }
    qint64 dt = timer.elapsed();
    qDebug("MainWindow::onTimeout(): dt=%lld ms, freeRunningCycles=%d", dt, freeRunningCycles);
}

void MainWindow::on_actionStep_triggered()
{
    QMutexLocker locker(&mutex);

    emu->run(1, false);
}

static const int TIMER_PERIOD_MS = 50;

void MainWindow::enableUiElements(bool enable) {
    ui->actionRun->setEnabled(enable);
    ui->actionStep->setEnabled(enable);
    ui->actionStop->setEnabled(!enable);
    ui->lineEdit_reg_a->setEnabled(enable);
    ui->lineEdit_reg_d->setEnabled(enable);
    ui->lineEdit_reg_pc->setEnabled(enable);
}

void MainWindow::on_actionRun_triggered()
{
    QMutexLocker locker(&mutex);

    // continue after an eventual breakpoint
    emu->run(1, false);

    timer->start(TIMER_PERIOD_MS);

    enableUiElements(false);

    running = true;

    grabKeyboard();
}

void MainWindow::doStop()
{
    QMutexLocker locker(&mutex);

    timer->stop();

    enableUiElements(true);

    running = false;

    releaseKeyboard();

    emu->poke(24576, 0);
}

void MainWindow::on_actionStop_triggered()
{
    doStop();
}

void MainWindow::on_actionReset_triggered()
{
    QMutexLocker locker(&mutex);

    emu->reset();

    memModel->refresh();

    ui->label_Key->setText(QString::number(emu->peek(24576)));
}

void MainWindow::on_horizontalSliderSpeed_valueChanged(int value)
{
    freeRunningCycles = pow(10.0, value/100.0);
    int cyclesPerSecond = freeRunningCycles*1000/TIMER_PERIOD_MS;
    ui->label_CLK_value->setText(QString::number(cyclesPerSecond));
    realtimeNotifications = cyclesPerSecond < 100000;
    emu->enableRealtimeNotifications(realtimeNotifications);
    //qWarning("MainWindow::on_horizontalSliderSpeed_valueChanged(): freeRunningCycles=%d", freeRunningCycles);
}

void MainWindow::on_actionOpen_triggered()
{
    QString startingPath = lastROMDir;
    if (startingPath.isEmpty()) {
        startingPath = QDir::currentPath();
    }
    QFileDialog fileDlg(this, "Select .hack ROM file", startingPath, "hack (*.hack);; All files (*.*)");
    fileDlg.setFileMode(QFileDialog::ExistingFile);
    if (fileDlg.exec() == QDialog::Accepted) {
        QString filename = fileDlg.selectedFiles().value(0);
        updateSettingsAndLoadROM(filename);
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
        case Qt::Key_F1:      return K_F1;
        case Qt::Key_F2:      return K_F2;
        case Qt::Key_F3:      return K_F3;
        case Qt::Key_F4:      return K_F4;
        case Qt::Key_F5:      return K_F5;
        case Qt::Key_F6:      return K_F6;
        case Qt::Key_F7:      return K_F7;
        case Qt::Key_F8:      return K_F8;
        case Qt::Key_F9:      return K_F9;
        case Qt::Key_F10:     return K_F10;
        case Qt::Key_F11:     return K_F11;
        case Qt::Key_F12:     return K_F12;

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

void MainWindow::on_checkBox_show_source_code_toggled(bool checked)
{
    int currPos = ui->tableViewCode->rowAt(0);

    codeModel->setShowSource(checked);
    ui->tableViewCode->scrollTo(codeModel->index(currPos, 0), QAbstractItemView::PositionAtTop);
}

void MainWindow::on_checkBox_hex_mode_toggled(bool checked)
{
    int currPos = ui->tableViewCode->rowAt(0);

    hexMode = checked;
    codeModel->setHexMode(hexMode);
    memModel->setHexMode(hexMode);
    emu->emitRegistersChanged();
    ui->tableViewCode->scrollTo(codeModel->index(currPos, 0), QAbstractItemView::PositionAtTop);
}

void MainWindow::on_checkBox_clear_RAM_on_reset_toggled(bool checked)
{
    clearRAMOnReset = checked;
    emu->setClearRAMOnReset(checked);
}

void MainWindow::on_tableViewCode_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        emu->toggleBreakpoint(index.row());
        codeModel->emitDataChanged(index.row());
    }
}
