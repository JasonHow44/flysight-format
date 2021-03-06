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

#include <QDir>
#include <QProcess>
#include <QRegExp>

#include "formatworker.h"

FormatWorker::FormatWorker(
        const QString &root,
        bool copy,
        const QString &audio):
    rootPath(root),
    copyAudio(copy),
    audioPath(audio)
{
    // Initialize here
}

FormatWorker::~FormatWorker()
{
    // Free resources
}

void FormatWorker::process()
{
    // Format
    QProcess *process = new QProcess;
    process->start(QString("cmd /c format %1 /q /y /fs:fat").arg(rootPath));
    process->waitForFinished();

    if (copyAudio)
    {
        // Create audio folder
        process->start(QString("cmd /c mkdir %1\\audio").arg(rootPath));
        process->waitForFinished();

        // Copy audio files
        process->start(QString("cmd /c copy %2 %1\\audio").arg(rootPath).arg(audioPath.replace('/', '\\')));
        process->waitForFinished();
    }

    // Let main thread know we're done
    emit finished(rootPath, copyAudio, audioPath);
}
