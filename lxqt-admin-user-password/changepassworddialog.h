#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class ChangePasswordDialog;
}

class ChangePasswordDialog : public QDialog
{
    Q_OBJECT

public:
    static int conversation(int, const struct pam_message**, struct pam_response**, void *);

    ChangePasswordDialog(QWidget */*parent*/ = 0);
    virtual ~ChangePasswordDialog();

    const QString &user() const;
    void setUser(const QString &user);

    void addWarning(const QString &);
    void addMessage(const QString &);
    void addLabel(const QString &, bool);

    const QList<QString> &values() const;
    bool changePassword();

protected:
    void reset();
    void showMessages();
    void showWarnings();

    virtual void showEvent(QShowEvent *event);

public slots:
    virtual void accept();
    virtual void reject();

protected:
    Ui::ChangePasswordDialog *_ui;

    bool _userAbort;

    QString _user;

    QStringList _warnings;
    QStringList _messages;
    QList<bool> _shadows;
    QList<QString> _values;
};

#endif // CHANGEPASSWORDDIALOG_H
