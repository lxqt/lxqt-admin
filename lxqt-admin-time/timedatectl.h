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

    QString timeZone() const;

    bool setTimeZone(QString timeZone, QString& errorMessage);
    bool setDateTime(QDateTime dateTime, QString& errorMessage);

private:
    QDBusInterface* mIface;
};

#endif // TIMEDATECTL_H
