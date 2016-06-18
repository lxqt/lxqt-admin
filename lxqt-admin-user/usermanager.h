#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

class QFileSystemWatcher;

class UserInfo
{
public:
    explicit UserInfo():mUid(0), mGid(0) {
    }
    explicit UserInfo(struct passwd* pw):
        mUid(pw->pw_uid),
        mGid(pw->pw_gid),
        mName(QString::fromLatin1(pw->pw_name)),
        mFullName(QString::fromUtf8(pw->pw_gecos)),
        mShell(QString::fromLocal8Bit(pw->pw_shell)),
        mHomeDir(QString::fromLocal8Bit(pw->pw_dir))
    {
    }

    uid_t uid()const {
        return mUid;
    }
    void setUid(uid_t uid) {
        mUid = uid;
    }

    gid_t gid() const {
        return mGid;
    }

    void setGid(gid_t gid) {
        mGid = gid;
    }

    QString name() const {
        return mName;
    }

    void setName(const QString& name) {
        mName = name;
    }

    QString fullName() const {
        return mFullName;
    }
    void setFullName(const QString& fullName) {
        mFullName = fullName;
    }

    QString shell() const {
        return mShell;
    }
    void setShell(const QString& shell) {
        mShell = shell;
    }

    QString homeDir() const {
        return mHomeDir;
    }
    void setHomeDir(const QString& homeDir) {
        mHomeDir = homeDir;
    }

    QByteArray passwd() const {
        return mPasswd;
    }

private:
    uid_t mUid;
    gid_t mGid;
    QString mName;
    QString mFullName;
    QString mShell;
    QString mHomeDir;
    QByteArray mPasswd;
};

class GroupInfo
{
public:
    explicit GroupInfo(): mGid(0) {
    }
    explicit GroupInfo(struct group* grp):
        mGid(grp->gr_gid),
        mName(grp->gr_name)
    {
    }

    gid_t gid() const {
        return mGid;
    }
    void setGid(gid_t gid) {
        mGid = gid;
    }

    QString name() const {
        return mName;
    }
    void setName(const QString& name) {
        mName = name;
    }

    const QStringList& members() const {
        return mMembers;
    }

    void setMembers(const QStringList& members) {
        mMembers = members;
    }

    void addMember(const QString& userName) {
        mMembers.append(userName);
    }

    void removeMember(const QString& userName) {
        mMembers.removeOne(userName);
    }

private:
    gid_t mGid;
    QString mName;
    QStringList mMembers;
};

class UserManager : public QObject
{
    Q_OBJECT
public:
    explicit UserManager(QObject *parent = 0);
    ~UserManager();

    const QList<UserInfo*>& users() const {
        return mUsers;
    }

    const QList<GroupInfo*>& groups() const {
        return mGroups;
    }

    const QStringList& availableShells();

    bool addUser(UserInfo* user);
    bool modifyUser(UserInfo* user, UserInfo* newSettings);
    bool deleteUser(UserInfo* user);

    bool addGroup(GroupInfo* group);
    bool modifyGroup(GroupInfo* group, GroupInfo* newSettings);
    bool deleteGroup(GroupInfo* group);

    // FIXME: add APIs to change group membership

    UserInfo* findUserInfo(const char* name);
    UserInfo* findUserInfo(QString name);
    UserInfo* findUserInfo(uid_t uid);

    GroupInfo* findGroupInfo(const char* name);
    GroupInfo* findGroupInfo(QString name);
    GroupInfo* findGroupInfo(gid_t gid);

private:
    void loadUsers();
    void loadGroups();
    bool pkexec(const QStringList &command);

Q_SIGNALS:
    void usersChanged();
    void groupsChanged();

protected Q_SLOTS:
    void onFileChanged(const QString &path);
    void reloadUsers();
    void reloadGroups();

private:
    QList<UserInfo*> mUsers;
    QList<GroupInfo*> mGroups;
    QFileSystemWatcher* mWatcher;
    QStringList mAvailableShells;
};

#endif // USERMANAGER_H
