/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#ifndef GROUPMANAGERDIALOG_H
#define GROUPMANAGERDIALOG_H

#include <QDialog>
#include "ui_groupmanagerdialog.h"
#include <glib.h>
#include <oobs/oobs-usersconfig.h>
#include <oobs/oobs-groupsconfig.h>

class GroupManagerDialog : public QDialog
{
    Q_OBJECT

public:
    GroupManagerDialog();
    ~GroupManagerDialog();

private Q_SLOTS:
    void onAddGroup();
    void onEditGroup();
    void onDeleteGroup();

private:
    OobsGroup* groupFromItem(QTreeWidgetItem *item);

private:
    Ui::GroupDialog ui;
    OobsGroupsConfig* mGroupsConfig;
};

#endif // GROUPMANAGERDIALOG_H
