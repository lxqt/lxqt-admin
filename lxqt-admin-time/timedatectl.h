#ifndef TIMEDATECTL_H
#define TIMEDATECTL_H

#include <QString>
#include <QDateTime>

class QDBusInterface;

class TimeDateCtl
{
public:
    explicit TimeDateCtl();
    ~TimeDateCtl();

    QString timeZone() const {
        return mTimeZone;
    }

    void setTimeZone(QString timeZone) {
        mTimeZone = timeZone;
        mTimeZoneChanged = true;
    }

    void setDateTime(QDateTime dateTime) {
        mDateTime = dateTime;
        mTimeChanged = true;
    }

    // really commit the changes to the system
    bool commit();

private:
    bool mTimeZoneChanged;
    QString mTimeZone;
    bool mTimeChanged;
    QDateTime mDateTime;
    QDBusInterface* mIface;
};

#endif // TIMEDATECTL_H
