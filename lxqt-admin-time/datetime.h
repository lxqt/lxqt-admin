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

#ifndef DATETIME_H
#define DATETIME_H

#include <QWidget>

namespace Ui {
class DateTime;
}

class DateTime : public QWidget
{
    Q_OBJECT

public:
    explicit DateTime(QWidget *parent = 0);
    ~DateTime();

    enum modified_enum {M_DATE = 0x1, M_TIME = 0x2};
    Q_DECLARE_FLAGS(modified_t, modified_enum)

    QDateTime dateTime() const;
    inline modified_t modified() const {return mModified;}

public Q_SLOTS:
    void reload();


private Q_SLOTS:
    void on_edit_time_userTimeChanged(const QTime &time);
    void timeout();
    void on_calendar_selectionChanged();

Q_SIGNALS:
    void changed(bool);

private:
    Ui::DateTime *ui;
    QTimer * mTimer;
    modified_t  mModified;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DateTime::modified_t)

#endif // DATETIME_H
