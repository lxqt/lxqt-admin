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

#include "groupdialog.h"
#include <QMessageBox>
#include "usermanager.h"

#define DEFAULT_GID_MIN 1000
#define DEFAULT_GID_MAX 32768

GroupDialog::GroupDialog(UserManager* userManager, GroupInfo* group, QWidget *parent, Qt::WindowFlags f):
    QDialog(parent, f),
    mUserManager(userManager),
    mGroup(group)
{
    ui.setupUi(this);
    ui.groupName->setText(group->name());
    ui.gid->setValue(group->gid());

#if 0
    GList* groupUsers = oobs_group_get_users(mGroup); // all users in this group
    // load all users
    OobsUsersConfig* usersConfig = OOBS_USERS_CONFIG(oobs_users_config_get());
    OobsList* users = oobs_users_config_get_users(usersConfig);
    if(users)
    {
        OobsListIter it;
        gboolean valid = oobs_list_get_iter_first(users, &it);
        while(valid)
        {
            OobsUser* user = OOBS_USER(oobs_list_get(users, &it));
            QListWidgetItem* item = new QListWidgetItem();
            item->setText(oobs_user_get_login_name(user));
            item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsSelectable);
            if(g_list_find(groupUsers, user)) // the user is in this group
                item->setCheckState(Qt::Checked);
            else
                item->setCheckState(Qt::Unchecked);
            QVariant obj = QVariant::fromValue<void*>(user);
            item->setData(Qt::UserRole, obj);
            ui.userList->addItem(item);
            valid = oobs_list_iter_next(users, &it);
        }
    }
    g_list_free(groupUsers);
#endif
}

GroupDialog::~GroupDialog()
{
}

void GroupDialog::accept()
{
    QString groupName = ui.groupName->text();
    if(groupName.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("The group name cannot be empty."));
        return;
    }
    mGroup->setName(groupName);
    mGroup->setGid(ui.gid->value());

    // update users
#if 0
    GList* groupUsers = oobs_group_get_users(mGroup); // all users in this group
    int rowCount = ui.userList->count();
    for(int row = 0; row < rowCount; ++row)
    {
        QListWidgetItem* item = ui.userList->item(row);
        QVariant obj = item->data(Qt::UserRole);
        OobsUser* user = OOBS_USER(obj.value<void*>());
        if(g_list_find(groupUsers, user)) // the user belongs to this group previously
        {
            if(item->checkState() == Qt::Unchecked) // it's unchecked, remove it
                oobs_group_remove_user(mGroup, user);
        }
        else // the user does not belong to this group previously
        {
            if(item->checkState() == Qt::Checked) // it's checked, we want it!
                oobs_group_add_user(mGroup, user);
        }
    }
    g_list_free(groupUsers);
#endif
    QDialog::accept();
}
