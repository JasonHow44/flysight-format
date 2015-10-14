#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QProcess>
#include <QRegExp>

#include "verifyworker.h"

VerifyWorker::VerifyWorker(
        const QString &root,
        const QString &audio):
    rootPath(root),
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
        emit failure(rootPath, audioPath);
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
            emit failure(rootPath, audioPath);
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
