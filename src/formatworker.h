#ifndef COPYWORKER_H
#define COPYWORKER_H

#include <QDir>
#include <QObject>
#include <QStringList>

class FormatWorker : public QObject
{
    Q_OBJECT
public:
    FormatWorker(const QString &root, const QString &audio);
    ~FormatWorker();

public slots:
    void process();

signals:
    void finished();

private:
    QString rootPath;
    QString audioPath;
};

#endif // COPYWORKER_H
