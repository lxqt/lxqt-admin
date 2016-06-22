#include "timedatectl.h"
#include <QProcess>
#include <QDebug>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QMessageBox>


TimeDateCtl::TimeDateCtl()
{
    mIface = new QDBusInterface(QStringLiteral("org.freedesktop.timedate1"),
                                QStringLiteral("/org/freedesktop/timedate1"),
                                QStringLiteral("org.freedesktop.timedate1"),
                                QDBusConnection::systemBus());
}

TimeDateCtl::~TimeDateCtl()
{
    delete mIface;
}

QString TimeDateCtl::timeZone() const
{
    return mIface->property("Timezone").toString();
}

bool TimeDateCtl::setTimeZone(QString timeZone, QString& errorMessage)
{
    mIface->call("SetTimezone", timeZone, true);
    QDBusError err = mIface->lastError();
    if(err.isValid())
    {
        errorMessage = err.message();
        return false;
    }
    return true;
}

bool TimeDateCtl::setDateTime(QDateTime dateTime, QString& errorMessage)
{
    // the timedatectl dbus service accepts "usec" input.
    // Qt can only get "msec"  => convert to usec here.
    mIface->call("SetTime", dateTime.toMSecsSinceEpoch() * 1000, false, true);
    QDBusError err = mIface->lastError();
    if(err.isValid())
    {
        errorMessage = err.message();
        return false;
    }
    return true;
}

bool TimeDateCtl::useNtp() const
{
    return mIface->property("NTP").toBool();
}

bool TimeDateCtl::setUseNtp(bool value, QString& errorMessage)
{
    mIface->call("SetNTP", value, true);
    QDBusError err = mIface->lastError();
    if(err.isValid())
    {
        errorMessage = err.message();
        return false;
    }
    return true;
}

bool TimeDateCtl::localRtc() const
{
    return mIface->property("LocalRTC").toBool();
}

bool TimeDateCtl::setLocalRtc(bool value, QString& errorMessage)
{
    mIface->call("SetLocalRTC", value, false, true);
    QDBusError err = mIface->lastError();
    if(err.isValid())
    {
        errorMessage = err.message();
        return false;
    }
    return true;
}
