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

#include "maindialog.h"
#include <QDebug>
#include <QMessageBox>
#include "userdialog.h"

MainDialog::MainDialog():
    QDialog(),
    mUserConfig(OOBS_USERS_CONFIG(oobs_users_config_get())),
    mGroupConfig(OOBS_GROUPS_CONFIG(oobs_groups_config_get()))
{
    ui.setupUi(this);
    connect(ui.addUser, SIGNAL(clicked(bool)), SLOT(onAddUser()));
    connect(ui.deleteUser, SIGNAL(clicked(bool)), SLOT(onDeleteUser()));
    connect(ui.editUser, SIGNAL(clicked(bool)), SLOT(onEditUser()));
    connect(ui.changePasswd, SIGNAL(clicked(bool)), SLOT(onChangePasswd()));
    connect(ui.manageGroups, SIGNAL(clicked(bool)), SLOT(onManageGroups()));

    // load user data
    oobs_object_update(OOBS_OBJECT(mUserConfig));
    oobs_object_update(OOBS_OBJECT(mGroupConfig));
    loadUsers();
    g_signal_connect(mUserConfig, "changed" , G_CALLBACK(onUsersConfigChanged), this);
}

MainDialog::~MainDialog()
{
    if(mUserConfig)
    {
        g_signal_handlers_disconnect_by_func(mUserConfig, (void*)G_CALLBACK(onUsersConfigChanged), this);
        g_object_unref(mUserConfig);
    }
    if(mGroupConfig)
        g_object_unref(mGroupConfig);
}

void MainDialog::loadUsers()
{
    ui.userList->clear();
    OobsList* users = oobs_users_config_get_users(mUserConfig);
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

void MainDialog::onUsersConfigChanged(OobsObject *obj, MainDialog *_this)
{
    qDebug() << "changed";
    _this->loadUsers();
}

OobsUser *MainDialog::userFromItem(QTreeWidgetItem *item)
{
    if(item)
    {
        QVariant obj = item->data(0, Qt::UserRole);
        OobsUser* user = OOBS_USER(qVariantValue<void*>(obj));
        return user;
    }
    return NULL;
}

void MainDialog::onAddUser()
{
    UserDialog dlg;
    dlg.exec();
}

void MainDialog::onDeleteUser()
{
    QTreeWidgetItem* item = ui.userList->currentItem();
    OobsUser* user = userFromItem(item);
    if(user)
    {
        if(QMessageBox::question(this, tr("Confirm"), tr("Are you sure you want to delete the user?"), QMessageBox::Ok|QMessageBox::Cancel) == QMessageBox::Ok)
            oobs_users_config_delete_user(mUserConfig, user);
    }
}

void MainDialog::onEditUser()
{
    QTreeWidgetItem* item = ui.userList->currentItem();
    OobsUser* user = userFromItem(item);
    if(user)
    {
        UserDialog dlg(user);
        dlg.exec();
    }
}

void MainDialog::onManageGroups()
{

}

void MainDialog::onChangePasswd()
{

}
