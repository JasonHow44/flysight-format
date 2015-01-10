#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>

#include <QMainWindow>

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
    bool nativeEvent(const QByteArray &eventType,
                     void *message, long *result);

private:
    Ui::MainWindow *ui;

    int  copyThreads;
    UINT queryCancelAutoPlay;

    bool isConfigFile(QString path);

    void handleDeviceInsert(int driveNum);
    void handleDeviceRemove(int driveNum);

private slots:
    void onFormatStarted();
    void onFormatFinished();
};

#endif // MAINWINDOW_H
