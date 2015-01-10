#include <windows.h>
#include <dbt.h>

#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QTextStream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    copyThreads(0)
{
    // Initialize UI object
    ui->setupUi(this);

    // Message for AutoRun notifications
    queryCancelAutoPlay = RegisterWindowMessage(L"QueryCancelAutoPlay");

    // Keep this window on top
    setWindowFlags(Qt::WindowStaysOnTopHint);
}

MainWindow::~MainWindow()
{
    // Destroy UI object
    delete ui;
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

void MainWindow::onFormatStarted()
{
    // Increment thread counter
    if (copyThreads++ == 0)
    {
        ui->statusBar->showMessage("Formatting disk...");
    }
}

void MainWindow::onFormatFinished()
{
    // Decrement thread counter
    if (--copyThreads == 0)
    {
        ui->statusBar->clearMessage();
    }
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
        // Create new process
        QProcess *process = new QProcess;

        // Connect start/end signals
        connect(process, SIGNAL(started()), this, SLOT(onFormatStarted()));
        connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onFormatFinished()));

        // Start formatting
        process->start(QString("cmd /c format %1 /q /y").arg(rootPath));
    }
}

void MainWindow::handleDeviceRemove(
        int driveNum)
{

}
