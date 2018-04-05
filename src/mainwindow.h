/***************************************************************************
**                                                                        **
**  FlySight Format                                                       **
**  Copyright 2018 Michael Cooper                                         **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>. **
**                                                                        **
****************************************************************************
**  Contact: Michael Cooper                                               **
**  Website: http://flysight.ca/                                          **
****************************************************************************/

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
