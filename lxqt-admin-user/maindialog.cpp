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
#include "ui_maindialog.h"
#include <QDebug>
#include <QMessageBox>

MainDialog::MainDialog():
    QDialog(),
    mUserConfig(OOBS_USERS_CONFIG(oobs_users_config_get())),
    mGroupConfig(OOBS_GROUPS_CONFIG(oobs_groups_config_get()))
{
    ui = new Ui::MainDialog;
    ui->setupUi(this);

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
    delete ui;
}

void MainDialog::loadUsers()
{
    ui->userList->clear();
    OobsList* users = oobs_users_config_get_users(mUserConfig);
    if(users)
    {
        OobsListIter it;
        gboolean valid = oobs_list_get_iter_first(users, &it);
        while(valid)
        {
            GObject* obj = oobs_list_get(users, &it);
            OobsUser* user = OOBS_USER(obj);
            if(!oobs_user_get_shell(user)) // FIXME: how can we exclude non-desktop users?
                continue;
            QString fullName = QString::fromUtf8(oobs_user_get_full_name(user));
            QString loginName = QString::fromLatin1(oobs_user_get_login_name(user));
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setData(0, Qt::DisplayRole, loginName);
            item->setData(1, Qt::DisplayRole, fullName);
            item->setData(2, Qt::DisplayRole, oobs_user_get_uid(user));
            ui->userList->addTopLevelItem(item);

            valid = oobs_list_iter_next(users, &it);
        }
    }
}

void MainDialog::onUsersConfigChanged(OobsObject *obj, MainDialog *_this)
{
    qDebug() << "changed";
    _this->loadUsers();
}
