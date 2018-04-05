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

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QProcess>
#include <QRegExp>

#include "verifyworker.h"

VerifyWorker::VerifyWorker(
        const QString &root,
        bool copy,
        const QString &audio):
    rootPath(root),
    copyAudio(copy),
    audioPath(audio)
{
    // Initialize here
}

VerifyWorker::~VerifyWorker()
{
    // Free resources
}

void VerifyWorker::process()
{
    // Check for existence of folder
    if (!QDir(QString("%1\\audio").arg(rootPath)).exists())
    {
        // Failure!
        emit failure(rootPath, copyAudio, audioPath);
        return;
    }

    QDirIterator it(audioPath);
    while (it.hasNext())
    {
        QDir dir(audioPath);
        QString path = QString("%1\\audio\\").arg(rootPath) + dir.relativeFilePath(it.next());

        QFileInfo checkFile(path);
        if (!checkFile.isFile()) continue;

        if (!checkFile.exists())
        {
            // Failure!
            emit failure(rootPath, copyAudio, audioPath);
            return;
        }
    }

    // Check files in folder
    QProcess *process = new QProcess;
    process->start(QString("cmd /c format %1 /q /y /fs:fat").arg(rootPath));
    process->waitForFinished();

    // Create audio folder
    process->start(QString("cmd /c mkdir %1\\audio").arg(rootPath));
    process->waitForFinished();

    // Copy audio files
    process->start(QString("cmd /c copy %2 %1\\audio").arg(rootPath).arg(audioPath.replace('/', '\\')));
    process->waitForFinished();

    // Success!
    emit success();
}
