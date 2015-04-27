#include <QDir>
#include <QProcess>
#include <QRegExp>

#include "formatworker.h"

FormatWorker::FormatWorker(
        const QString &root,
        const QString &audio):
    rootPath(root),
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
    process->start(QString("cmd /c format %1 /q /y").arg(rootPath));
    process->waitForFinished();

    // Create audio folder
    process->start(QString("cmd /c mkdir %1\\audio").arg(rootPath));
    process->waitForFinished();

    // Copy audio files
    process->start(QString("cmd /c copy %2 %1\\audio").arg(rootPath).arg(audioPath.replace('/', '\\')));
    process->waitForFinished();

    // Let main thread know we're done
    emit finished();
}
