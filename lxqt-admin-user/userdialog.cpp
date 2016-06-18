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

#include "userdialog.h"
#include <QMessageBox>
#include "usermanager.h"

#define DEFAULT_UID_MIN 1000
#define DEFAULT_UID_MAX 32768

UserDialog::UserDialog(UserManager* userManager, UserInfo* user, QWidget* parent):
    QDialog(parent),
    mUserManager(userManager),
    mUser(user),
    mFullNameChanged(false),
    mHomeDirChanged(false)
{
    ui.setupUi(this);

#if 0
    // load all groups
    OobsGroupsConfig* groupsConfig = OOBS_GROUPS_CONFIG(oobs_groups_config_get());
    OobsList* groups = oobs_groups_config_get_groups(groupsConfig);
    if(groups)
    {
        OobsListIter it;
        gboolean valid = oobs_list_get_iter_first(groups, &it);
        while(valid)
        {
            OobsGroup* group = OOBS_GROUP(oobs_list_get(groups, &it));
            ui.mainGroup->addItem(oobs_group_get_name(group));
            valid = oobs_list_iter_next(groups, &it);
        }
    }
#endif
    connect(ui.loginName, SIGNAL(textChanged(QString)), SLOT(onLoginNameChanged(QString)));

#if 0
    // add known shells to the combo box for selection
    GList* shells = oobs_users_config_get_available_shells(userConfig);
    for(GList* l = shells; l; l = l->next)
    {
        const char* shell = (const char*)l->data;
        ui.loginShell->addItem(QLatin1String(shell));
    }
#endif
    if(user) // edit an existing user
    {
        ui.loginName->setText(user->name());
        ui.changePasswd->setText(tr("Change password:"));
        ui.uid->setValue(user->uid());
        ui.fullName->setText(user->fullName());
        ui.loginShell->setEditText(user->shell());
        ui.homeDir->setText(user->homeDir());
        GroupInfo* group = userManager->findGroupInfo(user->gid());
        if(group)
            ui.mainGroup->setEditText(group->name());
    }
}

UserDialog::~UserDialog()
{
}

void UserDialog::onLoginNameChanged(const QString& text)
{
    if(!mFullNameChanged)
    {
        ui.fullName->blockSignals(true);
        ui.fullName->setText(text);
        ui.fullName->blockSignals(false);
    }

    if(!mHomeDirChanged)
    {
        ui.homeDir->blockSignals(true);
        ui.homeDir->setText("/home/" + text);
        ui.homeDir->blockSignals(false);
    }
}

void UserDialog::onFullNameChanged(const QString& text)
{
   mFullNameChanged = true;
}

void UserDialog::onHomeDirChanged(const QString& text)
{
    mHomeDirChanged = true;
}

void UserDialog::accept()
{
    mUser->setUid(ui.uid->value());
    bool createNew;
    QString loginName = ui.loginName->text();
    if(loginName.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("The user name cannot be empty."));
        return;
    }
    mUser->setFullName(ui.fullName->text());

#if 0
    // change password
    if(ui.changePasswd->isChecked())
    {
        QByteArray passwd = ui.passwd->text().toLatin1();
        if(passwd.isEmpty()) // show warnings if the password is empty
        {
            if(QMessageBox::warning(this, tr("Confirm"), tr("Are you sure you want to use an \"empty password\" for the user?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
                oobs_user_set_password_empty(mUser, true);
        }
        else
            oobs_user_set_password(mUser, passwd);
    }
#endif

    mUser->setHomeDir(ui.homeDir->text());

    // main group
    QString groupName = ui.mainGroup->currentText();
    if(!groupName.isEmpty()) {
        GroupInfo* group = mUserManager->findGroupInfo(groupName);
        if(group)
            mUser->setGid(group->gid());
    }
    QDialog::accept();
}
