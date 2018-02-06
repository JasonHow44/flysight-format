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

    int  formatThreads, verifyThreads, failures;
    UINT queryCancelAutoPlay;

    bool isConfigFile(QString path);

    void handleDeviceInsert(int driveNum);
    void handleDeviceRemove(int driveNum);

    void setAudioFolder(QString audioFolder);

    void updateStatusBar();

private slots:
    void on_browseButton_clicked();

    void onFormatStarted();
    void onFormatFinished(const QString &root, bool copyAudio, const QString &audio);

    void onVerifyStarted();
    void onVerifySuccess();
    void onVerifyFailure(const QString &root, bool copyAudio, const QString &audio);
};

#endif // MAINWINDOW_H
