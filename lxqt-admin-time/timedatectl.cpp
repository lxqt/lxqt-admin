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
    mTimeZone = mIface->property("Timezone").toString();
}

TimeDateCtl::~TimeDateCtl()
{
    delete mIface;
}

bool TimeDateCtl::commit()
{
    bool success = true;
    if(mTimeZoneChanged)
    {
        mIface->call("SetTimezone", mTimeZone, true);
        QDBusError err = mIface->lastError();
        if(err.isValid())
        {
            QMessageBox::critical(nullptr, QObject::tr("Failed to set timezone"), err.message());
        }
    }

    if(mTimeChanged)
    {
        mIface->call("SetTime", mDateTime.toMSecsSinceEpoch(), false, true);
        qDebug() << mIface->lastError();
        QDBusError err = mIface->lastError();
        if(err.isValid())
        {
            QMessageBox::critical(nullptr, QObject::tr("Failed to set time"), err.message());
        }
    }
    return success;
}
