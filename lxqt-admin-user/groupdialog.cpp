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

#define DEFAULT_GID_MIN 1000
#define DEFAULT_GID_MAX 32768

GroupDialog::GroupDialog(OobsGroup *group, QWidget *parent, Qt::WindowFlags f):
    QDialog(parent, f),
    mGroup(group ? OOBS_GROUP(g_object_ref(group)) : NULL)
{
    ui.setupUi(this);

    OobsGroupsConfig* groupsConfig = OOBS_GROUPS_CONFIG(oobs_groups_config_get());
    if(group) // edit an exiting group
    {
        ui.groupName->setReadOnly(true);
        ui.groupName->setText(oobs_group_get_name(group));
        mOldGId = oobs_group_get_gid(group);
        ui.gid->setValue(mOldGId);
    }
    else // create a new group
    {
        mOldGId = -1;
        ui.gid->setValue(oobs_groups_config_find_free_gid(groupsConfig, DEFAULT_GID_MIN, DEFAULT_GID_MAX));
    }

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
}

GroupDialog::~GroupDialog()
{
    if(mGroup)
        g_object_unref(mGroup);
}

void GroupDialog::accept()
{
    OobsGroupsConfig* groupsConfig = OOBS_GROUPS_CONFIG(oobs_groups_config_get());
    gid_t gid = ui.gid->value();
    if(gid != mOldGId && oobs_groups_config_is_gid_used(groupsConfig, gid))
    {
        QMessageBox::critical(this, tr("Error"), tr("The group ID is in use."));
        return;
    }

    if(!mGroup) // create a new group
    {
        QByteArray groupName = ui.groupName->text().toLatin1();
        if(groupName.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("The group name cannot be empty."));
            return;
        }
        if(oobs_groups_config_is_name_used(groupsConfig, groupName))
        {
            QMessageBox::critical(this, tr("Error"), tr("The group name is in use."));
            return;
        }
        mGroup = oobs_group_new(groupName);
    }
    oobs_group_set_gid(mGroup, gid);

    // update users
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

    oobs_object_commit(OOBS_OBJECT(mGroup));
    QDialog::accept();
}
