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

#include "groupdialog.h"
#include <QMessageBox>

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
        ui.gid->setValue(oobs_groups_config_find_free_gid(groupsConfig, 0, 32768));
    }
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
    oobs_object_commit(OOBS_OBJECT(mGroup));
    QDialog::accept();
}
