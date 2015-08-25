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

#include <LXQt/ConfigDialog>
#include <glib.h>
#include <oobs/oobs-timeconfig.h>
#include <oobs/oobs-ntpconfig.h>
#include <oobs/oobs-ntpserver.h>

class DateTime;
class Timezone;

class TimeAdminDialog: public LXQt::ConfigDialog
{
    Q_OBJECT

public:
    TimeAdminDialog(QWidget * parent = NULL) ;
    ~TimeAdminDialog();


    typedef enum  {M_TIMEDATE = 1 , M_TIMEZONE = 0x2} widgets_modified_enum;
    Q_DECLARE_FLAGS(widgets_modified_t, widgets_modified_enum)

protected:
    virtual void closeEvent(QCloseEvent  * event);

private Q_SLOTS:
    void onChanged(bool);

private:
    bool logInUser();
    void saveChangesToSystem();
    void loadTimeZones(QStringList & timeZones, QString & currentTimezone);
    void showChangedStar();

private:
    OobsTimeConfig* mTimeConfig;
    DateTime * mDateTimeWidget;
    Timezone * mTimezoneWidget;
    bool mUserLogedIn;
    QString mWindowTitle;
    widgets_modified_t mWidgetsModified;
};

Q_DECLARE_METATYPE(TimeAdminDialog::widgets_modified_enum)
Q_DECLARE_OPERATORS_FOR_FLAGS(TimeAdminDialog::widgets_modified_t)
