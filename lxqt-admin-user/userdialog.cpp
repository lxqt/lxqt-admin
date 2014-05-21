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

#include "userdialog.h"

UserDialog::UserDialog(OobsUser *user):
    QDialog(),
    mUser(user ? OOBS_USER(g_object_ref(user)) : NULL)
{
    ui.setupUi(this);
    
    if(user)
    {
        ui.loginName->setReadOnly(true);
        ui.passwordLabel->hide();
        ui.password->hide();
    }
    OobsUsersConfig* userConfig = OOBS_USERS_CONFIG(oobs_users_config_get());
    GList* shells = oobs_users_config_get_available_shells(userConfig);
    for(GList* l = shells; l; l = l->next)
    {
        const char* shell = (const char*)l->data;
        ui.loginShell->addItem(QLatin1String(shell));
    }

    if(user)
    {
      ui.loginName->setText(oobs_user_get_login_name(user));
      ui.uid->setValue(oobs_user_get_uid(user));
      ui.fullName->setText(oobs_user_get_full_name(user));
      ui.loginShell->setEditText(oobs_user_get_shell(user));
      ui.homeDir->setText(QString::fromLocal8Bit(oobs_user_get_home_directory(user)));
    }
    
}

UserDialog::~UserDialog()
{
    if(mUser)
        g_object_unref(mUser);
}
