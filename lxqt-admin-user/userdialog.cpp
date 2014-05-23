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

UserDialog::UserDialog(OobsUser* user, QWidget* parent):
    QDialog(),
    mFullNameChanged(false),
    mHomeDirChanged(false),
    mUser(user ? OOBS_USER(g_object_ref(user)) : NULL)
{
    ui.setupUi(this);

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

    connect(ui.loginName, SIGNAL(textChanged(QString)), SLOT(onLoginNameChanged(QString)));

    OobsUsersConfig* userConfig = OOBS_USERS_CONFIG(oobs_users_config_get());

    // add known shells to the combo box for selection
    GList* shells = oobs_users_config_get_available_shells(userConfig);
    for(GList* l = shells; l; l = l->next)
    {
        const char* shell = (const char*)l->data;
        ui.loginShell->addItem(QLatin1String(shell));
    }

    if(user) // edit an existing user
    {
        mOldUid = oobs_user_get_uid(user);
        ui.loginName->setReadOnly(true);
        ui.loginName->setText(oobs_user_get_login_name(user));
        ui.changePasswd->setText(tr("Change password:"));
        ui.uid->setValue(mOldUid);
        ui.fullName->setText(oobs_user_get_full_name(user));
        ui.loginShell->setEditText(oobs_user_get_shell(user));
        ui.homeDir->setText(QString::fromLocal8Bit(oobs_user_get_home_directory(user)));
        
        OobsGroup* group = oobs_user_get_main_group(user);
        ui.mainGroup->setEditText(oobs_group_get_name(group));
    }
    else // create a new user
    {
        mOldUid = -1;
        ui.loginName->setReadOnly(false);
        ui.loginName->setFocus();
        ui.changePasswd->setChecked(true);
        ui.uid->setValue(oobs_users_config_find_free_uid(userConfig, 1000, 32768));
        ui.loginShell->setEditText(oobs_users_config_get_default_shell(userConfig));
        ui.mainGroup->setCurrentIndex(-1);
    }
    
}

UserDialog::~UserDialog()
{
    if(mUser)
        g_object_unref(mUser);
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
    OobsUsersConfig* usersConfig = OOBS_USERS_CONFIG(oobs_users_config_get());
    uid_t uid = ui.uid->value();
    if(uid != mOldUid && oobs_users_config_is_uid_used(usersConfig, uid))
    {
        QMessageBox::critical(this, tr("Error"), tr("The user ID is in use."));
        return;
    }

    bool createNew;
    if(mUser)
        createNew = false;
    else
    {
        createNew = true;
        QByteArray loginName = ui.loginName->text().toLatin1();
        if(loginName.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("The user name cannot be empty."));
            return;
        }
        if(oobs_users_config_is_login_used(usersConfig, loginName))
        {
            QMessageBox::critical(this, tr("Error"), tr("The user name is in use."));
            return;
        }
        mUser = oobs_user_new(loginName);
    }
    oobs_user_set_uid(mUser, uid);

    QByteArray fullName = ui.fullName->text().toUtf8();
    oobs_user_set_full_name(mUser, fullName);

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

    QByteArray homeDir = ui.homeDir->text().toLocal8Bit();
    oobs_user_set_home_directory(mUser, homeDir);

    // main group
    OobsGroupsConfig* groupsConfig = OOBS_GROUPS_CONFIG(oobs_groups_config_get());
    QByteArray groupName = ui.mainGroup->currentText().toLatin1();
    OobsGroup* group = oobs_groups_config_get_from_name(groupsConfig, groupName);
    oobs_user_set_main_group(mUser, group);

    if(!createNew)
        oobs_object_commit_async(OOBS_OBJECT(mUser), NULL, NULL);
    QDialog::accept();
}
