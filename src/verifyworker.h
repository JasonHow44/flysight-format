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

#ifndef VERIFYWORKER_H
#define VERIFYWORKER_H

#include <QDir>
#include <QObject>
#include <QStringList>

class VerifyWorker : public QObject
{
    Q_OBJECT
public:
    VerifyWorker(const QString &root, bool copy, const QString &audio);
    ~VerifyWorker();

public slots:
    void process();

signals:
    void failure(const QString &root, bool copy, const QString &audio);
    void success();

private:
    QString rootPath;
    bool copyAudio;
    QString audioPath;
};

#endif // COPYWORKER_H
