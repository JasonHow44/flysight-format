#include <windows.h>
#include <dbt.h>

#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Initialize UI object
    ui->setupUi(this);

    // Message for AutoRun notifications
    queryCancelAutoPlay = RegisterWindowMessage(L"QueryCancelAutoPlay");
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

void MainWindow::handleDeviceInsert(
        int driveNum)
{
    QMessageBox::information(this, tr("Message"), tr("handleDeviceInsert"));
}

void MainWindow::handleDeviceRemove(
        int driveNum)
{
    QMessageBox::information(this, tr("Message"), tr("handleDeviceRemove"));
}
