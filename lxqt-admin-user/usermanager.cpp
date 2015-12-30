#include "usermanager.h"
#include <pwd.h>
#include <QDebug>

User::User(struct passwd* pw):
    mUid(pw->pw_uid),
    mName(QString::fromLatin1(pw->pw_name)),
    mShell(QString::fromLocal8Bit(pw->pw_shell)),
  mHomeDir(QString::fromLocal8Bit(pw->pw_dir))
{

}


UserManager::UserManager(QObject *parent) : QObject(parent)
{
    loadUsers();
    loadGroups();
}

void UserManager::loadUsers()
{
    while(struct passwd * pw = getpwent()) {
        qDebug() << pw->pw_name;
    }
}

void UserManager::loadGroups()
{

}
