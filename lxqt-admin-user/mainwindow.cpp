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

#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include "userdialog.h"

MainWindow::MainWindow():
    QMainWindow(),
    mUsersConfig(OOBS_USERS_CONFIG(oobs_users_config_get())),
    mGroupsConfig(OOBS_GROUPS_CONFIG(oobs_groups_config_get()))
{
    ui.setupUi(this);
    connect(ui.actionAdd, SIGNAL(triggered(bool)), SLOT(onAdd()));
    connect(ui.actionDelete, SIGNAL(triggered(bool)), SLOT(onDelete()));
    connect(ui.actionProperties, SIGNAL(triggered(bool)), SLOT(onEditProperties()));
    connect(ui.actionRefresh, SIGNAL(triggered(bool)), SLOT(onRefresh()));

    onRefresh(); // load the settings

    g_signal_connect(mUsersConfig, "changed" , G_CALLBACK(onUsersConfigChanged), this);
}

MainWindow::~MainWindow()
{
    if(mUsersConfig)
    {
        g_signal_handlers_disconnect_by_func(mUsersConfig, (void*)G_CALLBACK(onUsersConfigChanged), this);
        g_object_unref(mUsersConfig);
    }
    if(mGroupsConfig)
        g_object_unref(mGroupsConfig);
}

void MainWindow::loadUsers()
{
    ui.userList->clear();
    OobsList* users = oobs_users_config_get_users(mUsersConfig);
    if(users)
    {
        OobsListIter it;
        gboolean valid = oobs_list_get_iter_first(users, &it);
        while(valid)
        {
            GObject* obj = oobs_list_get(users, &it);
            OobsUser* user = OOBS_USER(obj);
            uid_t uid = oobs_user_get_uid(user);
            if(uid > 499 && oobs_user_get_shell(user)) // exclude system users
            {
                QString fullName = QString::fromUtf8(oobs_user_get_full_name(user));
                QString loginName = QString::fromLatin1(oobs_user_get_login_name(user));
                QTreeWidgetItem* item = new QTreeWidgetItem();
                item->setData(0, Qt::DisplayRole, loginName);
                QVariant obj = qVariantFromValue<void*>(user);
                item->setData(0, Qt::UserRole, obj);
                item->setData(1, Qt::DisplayRole, fullName);
                item->setData(2, Qt::DisplayRole, uid);
                ui.userList->addTopLevelItem(item);
            }
            valid = oobs_list_iter_next(users, &it);
        }
    }
}

void MainWindow::loadGroups()
{
    ui.groupList->clear();
    // load groups
    OobsList* groups = oobs_groups_config_get_groups(mGroupsConfig);
    if(groups)
    {
        OobsListIter it;
        gboolean valid = oobs_list_get_iter_first(groups, &it);
        while(valid)
        {
            OobsGroup* group = OOBS_GROUP(oobs_list_get(groups, &it));
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setData(0, Qt::DisplayRole, QString::fromLatin1(oobs_group_get_name(group)));
	    QVariant obj = qVariantFromValue<void*>(group);
            item->setData(0, Qt::UserRole, obj);
            item->setData(1, Qt::DisplayRole, oobs_group_get_gid(group));
            ui.groupList->addTopLevelItem(item);
            valid = oobs_list_iter_next(groups, &it);
        }
    }
}

void MainWindow::onUsersConfigChanged(OobsObject *obj, MainWindow *_this)
{
    qDebug() << "changed";
    _this->loadUsers();
}

OobsUser *MainWindow::userFromItem(QTreeWidgetItem *item)
{
    if(item)
    {
        QVariant obj = item->data(0, Qt::UserRole);
        OobsUser* user = OOBS_USER(qVariantValue<void*>(obj));
        return user;
    }
    return NULL;
}

OobsGroup* MainWindow::groupFromItem(QTreeWidgetItem *item)
{
    if(item)
    {
        QVariant obj = item->data(0, Qt::UserRole);
        return OOBS_GROUP(qVariantValue<void*>(obj));
    }
    return NULL;
}


void MainWindow::onAdd()
{
    if(ui.tabWidget->currentIndex() == PageUsers)
    {
	UserDialog dlg(NULL, this);
	dlg.exec();
    }
    else if (ui.tabWidget->currentIndex() == PageGroups)
    {
      
    }
}

void MainWindow::onDelete()
{
    QTreeWidgetItem* item = ui.userList->currentItem();
    if(ui.tabWidget->currentIndex() == PageUsers)
    {
	OobsUser* user = userFromItem(item);
	if(user)
	{
	    if(QMessageBox::question(this, tr("Confirm"), tr("Are you sure you want to delete the selected user?"), QMessageBox::Ok|QMessageBox::Cancel) == QMessageBox::Ok)
		oobs_users_config_delete_user(mUsersConfig, user);
	}
    }
    else if(ui.tabWidget->currentIndex() == PageGroups)
    {
	OobsGroup* group = groupFromItem(item);
	if(group)
	{
	    if(QMessageBox::question(this, tr("Confirm"), tr("Are you sure you want to delete the selected group?"), QMessageBox::Ok|QMessageBox::Cancel) == QMessageBox::Ok)
                oobs_groups_config_delete_group(mGroupsConfig, group);
	}
    }
}

void MainWindow::onEditProperties()
{
    QTreeWidgetItem* item = ui.userList->currentItem();
    if(ui.tabWidget->currentIndex() == PageUsers)
    {
	OobsUser* user = userFromItem(item);
	if(user)
	{
	    UserDialog dlg(user, this);
	    dlg.exec();
	}
    }
    else if(ui.tabWidget->currentIndex() == PageGroups)
    {
    }
}

void MainWindow::onRefresh()
{
    oobs_object_update(OOBS_OBJECT(mUsersConfig));
    loadUsers();
    oobs_object_update(OOBS_OBJECT(mGroupsConfig));
    loadGroups();
}
