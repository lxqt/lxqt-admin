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

    bool useNtp() const;
    bool setUseNtp(bool value, QString& errorMessage);

    bool localRtc() const;
    bool setLocalRtc(bool value, QString& errorMessage);

    QString timeZone() const;
    bool setTimeZone(QString timeZone, QString& errorMessage);

    bool setDateTime(QDateTime dateTime, QString& errorMessage);

private:
    QDBusInterface* mIface;
};

#endif // TIMEDATECTL_H
