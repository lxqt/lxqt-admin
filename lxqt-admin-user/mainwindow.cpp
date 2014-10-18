/*
 *
 * Copyright (C) 2014  Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
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
#include "groupdialog.h"

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
    g_signal_connect(mGroupsConfig, "changed" , G_CALLBACK(onGroupsConfigChanged), this);
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
                QString homeDir = QString::fromLocal8Bit(oobs_user_get_home_directory(user));
                QString groupName;
                OobsGroup* group = oobs_user_get_main_group(user);
                if(group)
                    groupName = QString::fromLatin1(oobs_group_get_name(group));

                QTreeWidgetItem* item = new QTreeWidgetItem();
                item->setData(0, Qt::DisplayRole, loginName);
                QVariant obj = QVariant::fromValue<void*>(user);
                item->setData(0, Qt::UserRole, obj);
                item->setData(1, Qt::DisplayRole, uid);
                item->setData(2, Qt::DisplayRole, fullName);
                item->setData(3, Qt::DisplayRole, groupName);
                item->setData(4, Qt::DisplayRole, homeDir);
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
            QVariant obj = QVariant::fromValue<void*>(group);
            item->setData(0, Qt::UserRole, obj);
            item->setData(1, Qt::DisplayRole, oobs_group_get_gid(group));
            ui.groupList->addTopLevelItem(item);
            valid = oobs_list_iter_next(groups, &it);
        }
    }
}

OobsUser *MainWindow::userFromItem(QTreeWidgetItem *item)
{
    if(item)
    {
        QVariant obj = item->data(0, Qt::UserRole);
        OobsUser* user = OOBS_USER(obj.value<void*>());
        return user;
    }
    return NULL;
}

OobsGroup* MainWindow::groupFromItem(QTreeWidgetItem *item)
{
    if(item)
    {
        QVariant obj = item->data(0, Qt::UserRole);
        return OOBS_GROUP(obj.value<void*>());
    }
    return NULL;
}

template <class T>
bool MainWindow::authenticate(T *obj)
{
    GError* err = NULL;
    if(!oobs_object_authenticate(OOBS_OBJECT(obj), &err))
    {
        if(err)
        {
            QMessageBox::critical(this, tr("Error"), QString::fromUtf8(err->message));
            g_error_free(err);
        }
        return false;
    }
    return true;
}

void MainWindow::onAdd()
{
    if(ui.tabWidget->currentIndex() == PageUsers)
    {
        if(authenticate(mUsersConfig))
        {
            UserDialog dlg(NULL, this);
            if(dlg.exec() == QDialog::Accepted)
            {
                OobsUser* user = dlg.user();
                oobs_users_config_add_user(mUsersConfig, user);
                oobs_object_commit(OOBS_OBJECT(mUsersConfig));
            }
        }
    }
    else if (ui.tabWidget->currentIndex() == PageGroups)
    {
        if(authenticate(mGroupsConfig))
        {
            GroupDialog dlg(NULL, this);
            if(dlg.exec() == QDialog::Accepted)
            {
                OobsGroup* group = dlg.group();
                oobs_groups_config_add_group(mGroupsConfig, group);
                oobs_object_commit(OOBS_OBJECT(mGroupsConfig));
            }
        }
    }
}

void MainWindow::onDelete()
{
    if(ui.tabWidget->currentIndex() == PageUsers)
    {
        QTreeWidgetItem* item = ui.userList->currentItem();
        OobsUser* user = userFromItem(item);
        if(user)
        {
            if(QMessageBox::question(this, tr("Confirm"), tr("Are you sure you want to delete the selected user?"), QMessageBox::Ok|QMessageBox::Cancel) == QMessageBox::Ok)
            {
                oobs_users_config_delete_user(mUsersConfig, user);
                oobs_object_commit(OOBS_OBJECT(mUsersConfig));
            }
        }
    }
    else if(ui.tabWidget->currentIndex() == PageGroups)
    {
        QTreeWidgetItem* item = ui.groupList->currentItem();
        OobsGroup* group = groupFromItem(item);
        if(group)
        {
            if(QMessageBox::question(this, tr("Confirm"), tr("Are you sure you want to delete the selected group?"), QMessageBox::Ok|QMessageBox::Cancel) == QMessageBox::Ok)
            {
                oobs_groups_config_delete_group(mGroupsConfig, group);
                oobs_object_commit(OOBS_OBJECT(mGroupsConfig));
            }
        }
    }
}

void MainWindow::onEditProperties()
{
    if(ui.tabWidget->currentIndex() == PageUsers)
    {
        QTreeWidgetItem* item = ui.userList->currentItem();
        OobsUser* user = userFromItem(item);
        if(user)
        {
            if(authenticate(mUsersConfig))
            {
                UserDialog dlg(user, this);
                dlg.exec();
            }
        }
    }
    else if(ui.tabWidget->currentIndex() == PageGroups)
    {
        QTreeWidgetItem* item = ui.groupList->currentItem();
        OobsGroup* group = groupFromItem(item);
        if(group)
        {
            if(authenticate(mGroupsConfig))
            {
                GroupDialog dlg(group, this);
                dlg.exec();
            }
        }
    }
}

void MainWindow::onRefresh()
{
    oobs_object_update(OOBS_OBJECT(mUsersConfig));
    loadUsers();
    oobs_object_update(OOBS_OBJECT(mGroupsConfig));
    loadGroups();
}
