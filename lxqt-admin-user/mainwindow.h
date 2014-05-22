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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"

#include <glib.h>
#include <oobs/oobs-usersconfig.h>
#include <oobs/oobs-groupsconfig.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
  enum
  {
      PageUsers = 0,
      PageGroups
  };
    
public:
    explicit MainWindow();
    virtual ~MainWindow();

private:
    void loadUsers();
    void loadGroups();
    static void onUsersConfigChanged(OobsObject* obj, MainWindow* _this);
    OobsUser* userFromItem(QTreeWidgetItem* item);
    OobsGroup* groupFromItem(QTreeWidgetItem *item);

private Q_SLOTS:
    void onAdd();
    void onDelete();
    void onEditProperties();
    void onRefresh();
    
private:
    Ui::MainWindow ui;
    OobsUsersConfig* mUsersConfig;
    OobsGroupsConfig* mGroupsConfig;
};

#endif // MAINWINDOW_H
