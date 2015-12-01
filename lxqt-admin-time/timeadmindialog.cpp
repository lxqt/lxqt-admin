/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014 LXQt team
 * Authors:
 *   Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "timeadmindialog.h"
#include <QLabel>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDateTime>
#include <QMap>
#include <QDebug>

#include "datetime.h"
#include "timezone.h"

#define ZONETAB_PATH "/usr/share/zoneinfo/zone.tab"

TimeAdminDialog::TimeAdminDialog(QWidget *parent):
    LXQt::ConfigDialog(tr("Time and date configuration"),new LXQt::Settings("TimeDate"), parent)
{
    setMinimumSize(QSize(400,400));
    mWindowTitle = windowTitle();


    mDateTimeWidget = new DateTime(this);
    addPage(mDateTimeWidget,tr("Date and time"));
    connect(this,SIGNAL(reset()),mDateTimeWidget,SLOT(reload()));
    connect(mDateTimeWidget,&DateTime::changed,this,&TimeAdminDialog::onChanged);
    mDateTimeWidget->setProperty("pModified",M_TIMEDATE);

    QStringList zones;
    QString currentZone;
    loadTimeZones(zones,currentZone);
    mTimezoneWidget = new Timezone(zones,currentZone,this);
    addPage(mTimezoneWidget,tr("Timezone"));
    connect(this,&TimeAdminDialog::reset,mTimezoneWidget,&Timezone::reload);
    connect(mTimezoneWidget,&Timezone::changed,this,&TimeAdminDialog::onChanged);
    mTimezoneWidget->setProperty("pModified",M_TIMEZONE);
}

TimeAdminDialog::~TimeAdminDialog()
{
}

void TimeAdminDialog::onChanged(bool ch)
{
    widgets_modified_enum flag = (widgets_modified_enum)
            sender()->property("pModified").toUInt();
    ch ? mWidgetsModified |= flag : mWidgetsModified &= ~flag;
    showChangedStar();
}


void TimeAdminDialog::showChangedStar()
{
    if (mWidgetsModified)
        setWindowTitle(mWindowTitle + "*");
    else
        setWindowTitle(mWindowTitle);
}

void TimeAdminDialog::closeEvent(QCloseEvent *event)
{
    //save changes to system
    if (mWidgetsModified)
    {
        saveChangesToSystem();
        event->accept();
    }
}

void TimeAdminDialog::loadTimeZones(QStringList & timeZones, QString & currentTimezone)
{
    currentTimezone = mTimeDateCtl.timeZone();

    timeZones.clear();
    QFile file(ZONETAB_PATH);
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray line;
        while(!file.atEnd())
        {
            line = file.readLine().trimmed();
            if(line.isEmpty() || line[0] == '#') // skip comments or empty lines
                continue;
            QList<QByteArray> items = line.split('\t');
            if(items.length() <= 2)
                continue;
            timeZones.append(QLatin1String(items[2]));
        }
        file.close();
    }
}



void TimeAdminDialog::saveChangesToSystem()
{
    QString timeZone = mTimezoneWidget->timezone();
    // FIXME: currently timezone settings does not work. is this a bug of system-tools-backend?
    if(!timeZone.isEmpty() && mWidgetsModified.testFlag(M_TIMEZONE)) {
        mTimeDateCtl.setTimeZone(timeZone);
    }

    if(mWidgetsModified.testFlag(M_TIMEDATE))
    {
        mTimeDateCtl.setDateTime(mDateTimeWidget->dateTime());
    }
    mTimeDateCtl.commit();
}
