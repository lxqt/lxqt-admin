#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <sys/types.h>

class User;
class Group;

class User
{
public:
    explicit User() {}
    explicit User(struct passwd* pw);

private:
    uid_t mUid;
    gid_t mGid;
    QString mName;
    QString mShell;
    QString mHomeDir;
    QByteArray mPasswd;
};

class Group
{
public:
private:
    uid_t mGid;
    QString mName;
};

class UserManager : public QObject
{
    Q_OBJECT
public:
    explicit UserManager(QObject *parent = 0);
    const QList<User>& users() const {
        return mUsers;
    }

    const QList<Group>& groups() const {
        return mGroups;
    }

private:
    void loadUsers();
    void loadGroups();

Q_SIGNALS:
    void changed();


private:
    QList<User> mUsers;
    QList<Group> mGroups;
};

#endif // USERMANAGER_H
