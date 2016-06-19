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
static const QString LOGIN_DEFS_FILE = QStringLiteral("/etc/login.defs");

UserManager::UserManager(QObject *parent):
    QObject(parent),
    mWatcher(new QFileSystemWatcher(QStringList() << PASSWD_FILE << GROUP_FILE, this))
{
    loadUsersAndGroups();
    connect(mWatcher, &QFileSystemWatcher::fileChanged, this, &UserManager::onFileChanged);
}

UserManager::~UserManager() {
    qDeleteAll(mUsers);
    qDeleteAll(mGroups);
}

void UserManager::loadUsersAndGroups()
{
    // load groups
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

    // load users
    setpwent();
    struct passwd * pw;
    while((pw = getpwent())) {
        UserInfo* user = new UserInfo(pw);
        mUsers.append(user);
        // add groups to this user
        for(const GroupInfo* group: mGroups) {
            if(group->hasMember(user->name())) {
                user->addGroup(group->name());
            }
        }
    }
    endpwent();
    std::sort(mUsers.begin(), mUsers.end(), [](UserInfo*& u1, UserInfo*& u2) {
        return u1->name() < u2->name();
    });
}

// load settings from /etc/login.defs
void UserManager::loadLoginDefs() {
    // FIXME: parse /etc/login.defs to get max UID, max system UID...etc.
    QFile file(LOGIN_DEFS_FILE);
    if(file.open(QIODevice::ReadOnly)) {
        while(!file.atEnd()) {
            QByteArray line = file.readLine().trimmed();
            if(line.isEmpty() || line.startsWith('#'))
                continue;
            QStringList parts = QString::fromUtf8(line).split(QRegExp("\\s"), QString::SkipEmptyParts);
            if(parts.length() >= 2) {
                QString& key = parts[0];
                QString& val = parts[1];
                if(key == QLatin1Literal("SYS_UID_MIN")) {
                }
                else if(key == QLatin1Literal("SYS_UID_MAX")) {
                }
                else if(key == QLatin1Literal("UID_MIN")) {
                }
                else if(key == QLatin1Literal("UID_MAX")) {
                }
                else if(key == QLatin1Literal("SYS_GID_MIN")) {
                }
                else if(key == QLatin1Literal("SYS_GID_MAX")) {
                }
                else if(key == QLatin1Literal("GID_MIN")) {
                }
                else if(key == QLatin1Literal("GID_MAX")) {
                }
            }
        }
        file.close();
    }
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

void UserManager::reload() {
    mWatcher->addPath(PASSWD_FILE);
    mWatcher->addPath(GROUP_FILE);

    qDeleteAll(mUsers);  // free the old UserInfo objects
    mUsers.clear();

    qDeleteAll(mGroups);  // free the old GroupInfo objects
    mGroups.clear();

    loadUsersAndGroups();
    Q_EMIT changed();
}

void UserManager::onFileChanged(const QString &path) {
    // QFileSystemWatcher is very broken and has a ridiculous design.
    // we get "fileChanged()" when the file is deleted or modified,
    // but there is no way to distinguish them. If the file is deleted,
    // the QFileSystemWatcher stop working silently. Hence we workaround
    // this by remove the paths from the watcher and add them back again
    // to force the creation of new notifiers.
    mWatcher->removePath(PASSWD_FILE);
    mWatcher->removePath(GROUP_FILE);
    QTimer::singleShot(500, this, &UserManager::reload);
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
        process.waitForStarted();
        process.write(stdinData);
        process.waitForBytesWritten();
        process.closeWriteChannel();
    }
    process.waitForFinished(-1);
    qDebug() << process.readAllStandardError();
    return process.exitCode() == 0;
}

bool UserManager::addUser(UserInfo* user) {
    if(!user || user->name().isEmpty())
        return false;
    QStringList command;
    command << QStringLiteral("useradd");
    if(user->uid() != 0) {
        command << QStringLiteral("-u") << QString::number(user->uid());
    }
    if(!user->homeDir().isEmpty()) {
        command << QStringLiteral("-d") << user->homeDir();
        command << QStringLiteral("-m"); // create the user's home directory if it does not exist.
    }
    if(!user->shell().isEmpty()) {
        command << QStringLiteral("-s") << user->shell();
    }
    if(!user->fullName().isEmpty()) {
        command << QStringLiteral("-c") << user->fullName();
    }
    if(user->gid() != 0) {
        command << QStringLiteral("-g") << QString::number(user->gid());
    }
    if(!user->groups().isEmpty()) {  // set group membership
        command << QStringLiteral("-G") << user->groups().join(',');
    }
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

    if(newSettings->groups() != user->groups()) {  // change group membership
        command << QStringLiteral("-G") << newSettings->groups().join(',');
    }

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
    if(!group || group->name().isEmpty())
        return false;

    QStringList command;
    command << QStringLiteral("groupadd");
    if(group->gid() != 0) {
        command << QStringLiteral("-g") << QString::number(group->gid());
    }
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
    if(!pkexec(command))
        return false;

    // if group members are changed, use gpasswd to reset members
    if(newSettings->members() != group->members()) {
        command.clear();
        command << QStringLiteral("gpasswd");
        command << QStringLiteral("-M");  // Set the list of group members.
        command << newSettings->members().join(',');
        command << group->name();
        return pkexec(command);
    }
    return true;
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

