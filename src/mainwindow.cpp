#include <windows.h>
#include <dbt.h>

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>
#include <QThread>

#include "formatworker.h"
#include "mainwindow.h"
#include "verifyworker.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    formatThreads(0),
    verifyThreads(0),
    failures(0)
{
    // Initialize UI object
    ui->setupUi(this);

    // Message for AutoRun notifications
    queryCancelAutoPlay = RegisterWindowMessage(L"QueryCancelAutoPlay");

    // Initialize settings object
    QSettings settings("FlySight", "Format");

    // Get audio folder
    QString audioFolder = settings.value("audioFolder").toString();

    if (!audioFolder.isEmpty())
    {
        // Update audio folder
        setAudioFolder(audioFolder);
    }
    else
    {
        // Use temporary folder
        setAudioFolder(QDir::tempPath());
    }

    // Keep this window on top
    setWindowFlags(Qt::WindowStaysOnTopHint);
}

MainWindow::~MainWindow()
{
    // Initialize settings object
    QSettings settings("FlySight", "Format");

    // Write INI filename
    settings.setValue("audioFolder", ui->dstEdit->text());

    // Destroy UI object
    delete ui;
}

void MainWindow::setAudioFolder(
        QString audioFolder)
{
    QDir audioDir(audioFolder);

    // Update dialog box
    ui->dstEdit->setText(audioFolder);
}

void MainWindow::on_browseButton_clicked()
{
    // Get new settings file
    QString audioFolder = QFileDialog::getExistingDirectory(
                this,
                "",
                ui->dstEdit->text());

    if (!audioFolder.isEmpty())
    {
        // Update audio folder
        setAudioFolder(audioFolder);
    }
}

bool MainWindow::nativeEvent(
        const QByteArray &, // unused
        void             *message,
        long             *result)
{
    MSG *msg = (MSG *) message;

    if (msg->message == WM_DEVICECHANGE)
    {
        bool handle = false;

        switch (msg->wParam)
        {
        case DBT_DEVICEARRIVAL:
            handle = true;
            break;
        case DBT_DEVICEREMOVECOMPLETE:
            handle = true;
            break;
        }

        if (handle && msg->lParam)
        {
            DEV_BROADCAST_HDR *pHdr = (DEV_BROADCAST_HDR *) msg->lParam;

            if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
            {
                DEV_BROADCAST_VOLUME *pVol = (DEV_BROADCAST_VOLUME *) pHdr;

                for (int driveNum = 0, mask = pVol->dbcv_unitmask;
                     mask;
                     ++driveNum, mask >>= 1)
                {
                    if (mask & 1)
                    {
                        switch (msg->wParam)
                        {
                        case DBT_DEVICEARRIVAL:
                            handleDeviceInsert(driveNum);
                            break;
                        case DBT_DEVICEREMOVECOMPLETE:
                            handleDeviceRemove(driveNum);
                            break;
                        }
                    }
                }
            }
        }
    }

    if (msg->message == queryCancelAutoPlay)
    {
        // Disable AutoRun
        *(result) = 1;
        return true;
    }

    return false;
}

void MainWindow::updateStatusBar()
{
    QString status;

    if (formatThreads > 0)
    {
        status += tr("Formatting %1 disk%2")
                .arg(formatThreads)
                .arg(formatThreads > 1 ? "s" : "");
    }

    if (verifyThreads > 0)
    {
        if (!status.isEmpty()) status += "; ";
        status += tr("Verifying %1 disk%2")
                .arg(verifyThreads)
                .arg(verifyThreads > 1 ? "s" : "");
    }

    if (failures > 0)
    {
        if (!status.isEmpty()) status += "; ";
        status += tr("%1 failure%2")
                .arg(failures)
                .arg(failures > 1 ? "s" : "");
    }

    if (status.isEmpty())
    {
        ui->statusBar->clearMessage();
    }
    else
    {
        ui->statusBar->showMessage(status);
    }
}

void MainWindow::onFormatStarted()
{
    // Increment thread counter
    formatThreads++;

    // Update format/verify count
    updateStatusBar();
}

void MainWindow::onFormatFinished(const QString &root, const QString &audio)
{
    // Decrement thread counter
    formatThreads--;

    // Create worker thread
    QThread *thread = new QThread;

    // Create worker thread controller
    VerifyWorker *worker = new VerifyWorker(root, audio);
    worker->moveToThread(thread);

    // Connect worker thread to controller
    connect(thread, SIGNAL(started()), this, SLOT(onVerifyStarted()));
    connect(thread, SIGNAL(started()),  worker, SLOT(process()));
    connect(worker, SIGNAL(success()), thread, SLOT(quit()));
    connect(worker, SIGNAL(success()), worker, SLOT(deleteLater()));
    connect(worker, SIGNAL(success()), this, SLOT(onVerifySuccess()));
    connect(worker, SIGNAL(failure(const QString &, const QString &)), thread, SLOT(quit()));
    connect(worker, SIGNAL(failure(const QString &, const QString &)), worker, SLOT(deleteLater()));
    connect(worker, SIGNAL(failure(const QString &, const QString &)), this, SLOT(onVerifyFailure(const QString &, const QString &)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    // Start worker thread
    thread->start();
}

void MainWindow::onVerifyStarted()
{
    // Increment thread counter
    verifyThreads++;

    // Update format/verify count
    updateStatusBar();
}

void MainWindow::onVerifySuccess()
{
    // Increment thread counter
    verifyThreads--;

    // Update format/verify count
    updateStatusBar();
}

void MainWindow::onVerifyFailure(const QString &root, const QString &audio)
{
    // Increment thread counter
    verifyThreads--;

    // Increment failure count
    failures++;

    // Create worker thread
    QThread *thread = new QThread;

    // Create worker thread controller
    FormatWorker *worker = new FormatWorker(root, audio);
    worker->moveToThread(thread);

    // Connect worker thread to controller
    connect(thread, SIGNAL(started()), this, SLOT(onFormatStarted()));
    connect(thread, SIGNAL(started()),  worker, SLOT(process()));
    connect(worker, SIGNAL(finished(const QString &, const QString &)), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished(const QString &, const QString &)), worker, SLOT(deleteLater()));
    connect(worker, SIGNAL(finished(const QString &, const QString &)), this, SLOT(onFormatFinished(const QString &, const QString &)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    // Start worker thread
    thread->start();
}

bool MainWindow::isConfigFile(QString path)
{
    // Open the file if it exists
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    // Create text stream
    QTextStream in(&file);

    // Read a line from the file
    QString line = in.readLine();

    // Compare it with reference text
    QString version = "; Firmware version";
    return line.left(version.length()) == version;
}

void MainWindow::handleDeviceInsert(
        int driveNum)
{
    // Convert drive number to letter
    QString rootPath = QString("%1:")
            .arg(QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ").at(driveNum));

    // Get configuration file name
    QString configPath(rootPath + QString("CONFIG.TXT"));

    // If the configuration file exists
    if (isConfigFile(configPath))
    {
        // Create worker thread
        QThread *thread = new QThread;

        // Create worker thread controller
        FormatWorker *worker = new FormatWorker(rootPath, ui->dstEdit->text());
        worker->moveToThread(thread);

        // Connect worker thread to controller
        connect(thread, SIGNAL(started()), this, SLOT(onFormatStarted()));
        connect(thread, SIGNAL(started()),  worker, SLOT(process()));
        connect(worker, SIGNAL(finished(const QString &, const QString &)), thread, SLOT(quit()));
        connect(worker, SIGNAL(finished(const QString &, const QString &)), worker, SLOT(deleteLater()));
        connect(worker, SIGNAL(finished(const QString &, const QString &)), this, SLOT(onFormatFinished(const QString &, const QString &)));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        // Start worker thread
        thread->start();
    }
}

void MainWindow::handleDeviceRemove(
        int driveNum)
{

}
