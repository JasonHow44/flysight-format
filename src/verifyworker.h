#ifndef VERIFYWORKER_H
#define VERIFYWORKER_H

#include <QDir>
#include <QObject>
#include <QStringList>

class VerifyWorker : public QObject
{
    Q_OBJECT
public:
    VerifyWorker(const QString &root, const QString &audio);
    ~VerifyWorker();

public slots:
    void process();

signals:
    void failure(const QString &root, const QString &audio);
    void success();

private:
    QString rootPath;
    QString audioPath;
};

#endif // COPYWORKER_H
