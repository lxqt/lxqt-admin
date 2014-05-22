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

#include "groupmanagerdialog.h"

GroupManagerDialog::GroupManagerDialog():
    QDialog(),
    mGroupsConfig(OOBS_GROUPS_CONFIG(oobs_groups_config_get()))
{
    ui.setupUi(this);
    connect(ui.addGroup, SIGNAL(clicked(bool)), SLOT(onAddGroup()));
    connect(ui.editGroup, SIGNAL(clicked(bool)), SLOT(onEditGroup()));
    connect(ui.deleteGroup, SIGNAL(clicked(bool)), SLOT(onDeleteGroup()));

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

GroupManagerDialog::~GroupManagerDialog()
{
}

OobsGroup* GroupManagerDialog::groupFromItem(QTreeWidgetItem *item)
{
    if(item)
    {
        QVariant obj = item->data(0, Qt::UserRole);
        return OOBS_GROUP(qVariantValue<void*>(obj));
    }
    return NULL;
}


void GroupManagerDialog::onAddGroup()
{

}

void GroupManagerDialog::onEditGroup()
{
    QTreeWidgetItem* item = ui.groupList->currentItem();
    OobsGroup* group = groupFromItem(item);
    if(group)
    {
    }
}

void GroupManagerDialog::onDeleteGroup()
{
    QTreeWidgetItem* item = ui.groupList->currentItem();
    OobsGroup* group = groupFromItem(item);
    if(group)
        oobs_groups_config_delete_group(mGroupsConfig, group);
}
