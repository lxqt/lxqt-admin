#include "timedatectl.h"
#include <QProcess>
#include <QDebug>

TimeDateCtl::TimeDateCtl()
{
    QProcess timedatectl;
    timedatectl.start(QStringLiteral("timedatectl"));
    timedatectl.waitForFinished();
    while(!timedatectl.atEnd()) {
        QString line = timedatectl.readLine().trimmed();
        int findTZ = line.indexOf(QLatin1String("Time zone:"));
        if(findTZ != -1) {
            findTZ += 10;
            while(line[findTZ] == ' ')
                ++findTZ;
            int space = line.indexOf(' ', findTZ);
            mTimeZone = line.mid(findTZ, space - findTZ);
            break;
        }
    }
    timedatectl.close();
}

bool TimeDateCtl::commit()
{
    bool success = true;
    QProcess timedatectl;
    QStringList args;
    if(mTimeZoneChanged) {
        timedatectl.start(QStringLiteral("timedatectl"), QStringList() << "set-timezone" << mTimeZone);
        timedatectl.waitForFinished();
        success = timedatectl.exitCode() == 0;
    }
    if(!success)
        return success;

    if(mTimeChanged) {
        timedatectl.start(QStringLiteral("timedatectl"), QStringList() << "set-time" << mDateTime.toString("yyyy-MM-dd hh:mm:ss"));
        timedatectl.waitForFinished();
        success = timedatectl.exitCode() == 0;
    }
    return success;
}
