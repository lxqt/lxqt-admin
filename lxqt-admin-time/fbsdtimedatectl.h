#ifndef ITIMEDATECTL_H
#include "itimedatectl.h"
#endif
#ifndef FBSDTIMEDATECTL_H
#define FBSDTIMEDATECTL_H
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QProcess>
extern "C" {
#include <errno.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/fcntl.h>
#include <sys/time.h>
}
#define	_PATH_ZONETAB		"/usr/share/zoneinfo/zone.tab"
#define	_PATH_ZONEINFO		"/usr/share/zoneinfo"
#define	_PATH_LOCALTIME		"/etc/localtime"
#define	_PATH_DB		"/var/db/zoneinfo"
#define	_PATH_WALL_CMOS_CLOCK	"/etc/wall_cmos_clock"

class FBSDTimeDateCtl : public ITimeDateCtl
{
public:
    FBSDTimeDateCtl();
    QString timeZone() const;
    bool useNtp() const;
    bool localRtc() const;
    bool setTimeZone(QString timeZone, QString& errorMessage);
    bool setUseNtp(bool value, QString& errorMessage);
    bool setLocalRtc(bool value, QString& errorMessage);
    bool setDateTime(QDateTime dateTime, QString& errorMessage);
};

#endif // FBSDTIMEDATECTL_H
