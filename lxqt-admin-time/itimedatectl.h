#ifndef ITIMEDATECTL_H
#define ITIMEDATECTL_H
#include <QString>
#include <QDateTime>
class ITimeDateCtl
{

public:
    ITimeDateCtl();
    virtual ~ITimeDateCtl();
    virtual QString timeZone() const = 0;
    virtual bool useNtp() const = 0;
    virtual bool localRtc() const = 0;
    virtual bool setTimeZone(QString timeZone, QString& errorMessage) = 0;
    virtual bool setUseNtp(bool value, QString& errorMessage)=0;
    virtual bool setLocalRtc(bool value, QString& errorMessage)=0;
    virtual bool setDateTime(QDateTime dateTime, QString& errorMessage)=0;

};
#endif



