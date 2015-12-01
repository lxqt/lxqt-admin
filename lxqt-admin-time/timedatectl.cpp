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
    QProcess timedatectl;
    QStringList args;
    if(mTimeZoneChanged) {
        args << "set-timezone" << mTimeZone;
    }
    timedatectl.start(QStringLiteral("timedatectl"), args);
    timedatectl.waitForFinished();
    return timedatectl.exitCode() == 0;
}
