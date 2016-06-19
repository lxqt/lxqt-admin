#include "usermanager.h"
#include <QDebug>
#include <algorithm>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QProcess>
#include <QFile>
#include <unistd.h>

static const QString PASSWD_FILE = QStringLiteral("/etc/passwd");
static const QString GROUP_FILE = QStringLiteral("/etc/group");

UserManager::UserManager(QObject *parent):
    QObject(parent),
    mWatcher(new QFileSystemWatcher(QStringList() << PASSWD_FILE << GROUP_FILE, this))
{
    loadUsers();
    loadGroups();
    connect(mWatcher, &QFileSystemWatcher::fileChanged, this, &UserManager::onFileChanged);
}

UserManager::~UserManager() {
    qDeleteAll(mUsers);
    qDeleteAll(mGroups);
}

void UserManager::loadUsers()
{
    setpwent();
    struct passwd * pw;
    while((pw = getpwent())) {
        UserInfo* user = new UserInfo(pw);
        mUsers.append(user);
        // qDebug() << pw->pw_name << pw->pw_gecos;
    }
    endpwent();
    std::sort(mUsers.begin(), mUsers.end(), [](UserInfo*& u1, UserInfo*& u2) {
        return u1->name() < u2->name();
    });
}

void UserManager::loadGroups()
{
    setgrent();
    struct group * grp;
    while((grp = getgrent())) {
        GroupInfo* group = new GroupInfo(grp);
        mGroups.append(group);
        // add members of this group
        for(char** member_name = grp->gr_mem; *member_name; ++member_name) {
            group->addMember(QString::fromLatin1(*member_name));
        }
    }
    endgrent();
    std::sort(mGroups.begin(), mGroups.end(), [](GroupInfo* g1, GroupInfo* g2) {
        return g1->name() < g2->name();
    });
}

UserInfo* UserManager::findUserInfo(const char* name) {
    auto it = std::find_if(mUsers.begin(), mUsers.end(), [name](const UserInfo* user) {
        return user->name() == name;
    });
    return it != mUsers.end() ? *it : nullptr;
}

UserInfo* UserManager::findUserInfo(QString name) {
    auto it = std::find_if(mUsers.begin(), mUsers.end(), [name](const UserInfo* user) {
        return user->name() == name;
    });
    return it != mUsers.end() ? *it : nullptr;
}

UserInfo* UserManager::findUserInfo(uid_t uid) {
    auto it = std::find_if(mUsers.begin(), mUsers.end(), [uid](const UserInfo* user) {
        return user->uid() == uid;
    });
    return it != mUsers.end() ? *it : nullptr;
}

GroupInfo* UserManager::findGroupInfo(const char* name) {
    auto it = std::find_if(mGroups.begin(), mGroups.end(), [name](const GroupInfo* group) {
        return group->name() == name;
    });
    return it != mGroups.end() ? *it : nullptr;
}

GroupInfo* UserManager::findGroupInfo(QString name) {
    auto it = std::find_if(mGroups.begin(), mGroups.end(), [name](const GroupInfo* group) {
        return group->name() == name;
    });
    return it != mGroups.end() ? *it : nullptr;
}

GroupInfo* UserManager::findGroupInfo(gid_t gid) {
    auto it = std::find_if(mGroups.begin(), mGroups.end(), [gid](const GroupInfo* group) {
        return group->gid() == gid;
    });
    return it != mGroups.end() ? *it : nullptr;
}

void UserManager::reloadUsers() {
    mWatcher->addPath(PASSWD_FILE);

    qDeleteAll(mUsers);  // free the old User objects
    mUsers.clear();
    loadUsers();
    Q_EMIT usersChanged();
}

void UserManager::reloadGroups() {
    mWatcher->addPath(GROUP_FILE);

    qDeleteAll(mGroups);  // free the old User objects
    mGroups.clear();
    loadUsers();
    Q_EMIT groupsChanged();
}

void UserManager::onFileChanged(const QString &path) {
    if(path == PASSWD_FILE) {
        QTimer::singleShot(500, this, &UserManager::reloadUsers);
        mWatcher->removePath(PASSWD_FILE);
    }
    else {
        QTimer::singleShot(500, this, &UserManager::reloadGroups);
        mWatcher->removePath(GROUP_FILE);
    }
}

bool UserManager::pkexec(const QStringList& command, const QByteArray& stdinData) {
    QProcess process;
    qDebug() << command;
    QStringList args;
    args << QStringLiteral("--disable-internal-agent")
        << QStringLiteral("lxqt-admin-user-helper")
        << command;
    process.start(QStringLiteral("pkexec"), args);
    if(!stdinData.isEmpty()) {
        qDebug() << stdinData;
        process.waitForStarted();
        process.write(stdinData);
        process.waitForBytesWritten();
        process.closeWriteChannel();
    }
    process.waitForFinished(-1);
    qDebug() << process.readAllStandardError();
    qDebug() << process.readAllStandardOutput();
    return process.exitCode() == 0;
}

bool UserManager::addUser(UserInfo* user) {
    if(!user || user->name().isEmpty() || 0 == user->uid())
        return false;

    // FIXME: parse /etc/login.defs to get max UID, max system UID...etc.

    QStringList command;
    command << QStringLiteral("useradd");
    command << QStringLiteral("-u") << QString::number(user->uid());

    if(!user->homeDir().isEmpty()) {
        command << QStringLiteral("-d") << user->homeDir();
        command << QStringLiteral("-m"); // create the user's home directory if it does not exist.
    }
    if(!user->shell().isEmpty()) {
        command << QStringLiteral("-s") << user->shell();
    }

    if(!user->fullName().isEmpty())
        command << QStringLiteral("-c") << user->fullName();
    if(user->gid() != 0)
        command << QStringLiteral("-g") << QString::number(user->gid());

    command << user->name();
    return pkexec(command);
}

bool UserManager::modifyUser(UserInfo* user, UserInfo* newSettings) {
    if(!user || user->name().isEmpty() || !newSettings)
        return false;

    QStringList command;
    command << QStringLiteral("usermod");
    if(newSettings->uid() != user->uid())
        command << QStringLiteral("-u") << QString::number(newSettings->uid());

    if(newSettings->homeDir() != user->homeDir()) {
        command << QStringLiteral("-d") << newSettings->homeDir();
        // command << QStringLiteral("-m"); // create the user's home directory if it does not exist.
    }
    if(newSettings->shell() != user->shell()) {
        command << QStringLiteral("-s") << newSettings->shell();
    }

    if(newSettings->fullName() != user->fullName())
        command << QStringLiteral("-c") << newSettings->fullName();
    if(newSettings->gid() != user->gid())
        command << QStringLiteral("-g") << QString::number(newSettings->gid());

    if(newSettings->name() != user->name())  // change login name
        command << QStringLiteral("-l") << newSettings->name();

    command << user->name();
    return pkexec(command);
}

bool UserManager::deleteUser(UserInfo* user) {
    if(!user || user->name().isEmpty())
        return false;

    QStringList command;
    command << QStringLiteral("userdel");
    command << user->name();
    return pkexec(command);
}

bool UserManager::changePassword(UserInfo* user, QByteArray newPasswd) {
    // In theory, the current user should be able to use "passwd" to
    // reset his/her own password without root permission, but...
    // /usr/bin/passwd is a setuid program running as root and QProcess
    // does not seem to capture its stdout... So... requires root for now.
    if(geteuid() == user->uid()) {
        // FIXME: there needs to be a way to let a user change his/her own password.
        // Maybe we can use our pkexec helper script to achieve this.
    }
    QStringList command;
    command << QStringLiteral("passwd");
    command << user->name();

    // we need to type the new password for two times.
    QByteArray stdinData;
    stdinData += newPasswd;
    stdinData += "\n";
    stdinData += newPasswd;
    stdinData += "\n";
    return pkexec(command, stdinData);
}

bool UserManager::addGroup(GroupInfo* group) {
    if(!group || group->name().isEmpty() || 0 == group->gid())
        return false;

    QStringList command;
    command << QStringLiteral("groupadd");
    command << QStringLiteral("-g") << QString::number(group->gid());
    command << group->name();
    return pkexec(command);
}

bool UserManager::modifyGroup(GroupInfo* group, GroupInfo* newSettings) {
    if(!group || group->name().isEmpty() || !newSettings)
        return false;
    QStringList command;
    command << QStringLiteral("groupmod");
    if(newSettings->gid() != group->gid())
        command << QStringLiteral("-g") << QString::number(newSettings->gid());
    if(newSettings->name() != group->name())
        command << QStringLiteral("-n") << newSettings->name();
    command << group->name();
    return pkexec(command);
}

bool UserManager::deleteGroup(GroupInfo* group) {
    if(!group || group->name().isEmpty())
        return false;
    QStringList command;
    command << QStringLiteral("groupdel");
    command << group->name();
    return pkexec(command);
}

bool UserManager::changePassword(GroupInfo* group, QByteArray newPasswd) {
    QStringList command;
    command << QStringLiteral("gpasswd");
    command << group->name();

    // we need to type the new password for two times.
    QByteArray stdinData = newPasswd;
    stdinData += "\n";
    stdinData += newPasswd;
    stdinData += "\n";
    return pkexec(command, stdinData);
}


const QStringList& UserManager::availableShells() {
    if(mAvailableShells.isEmpty()) {
        QFile file("/etc/shells");
        if(file.open(QIODevice::ReadOnly)) {
            while(!file.atEnd()) {
                QByteArray line = file.readLine().trimmed();
                if(line.isEmpty() || line.startsWith('#'))
                    continue;
                mAvailableShells.append(QString::fromLocal8Bit(line));
            }
            file.close();
        }
    }
    return mAvailableShells;
}

