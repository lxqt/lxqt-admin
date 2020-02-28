#include <QCommandLineParser>
#include <QMessageBox>
#include <QTextStream>
#include <QIcon>
#include <LXQt/Application>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include "changepassworddialog.h"

int main(int argc, char **argv)
{
    QApplication::setSetuidAllowed(true);
    LXQt::Application app(argc, argv, true);
    app.setQuitOnLastWindowClosed(false);

    QApplication::setApplicationName(QSL(PROJECT_NAME));
    QCommandLineParser parser;
    QString user;

    parser.setApplicationDescription(QObject::tr("LXQt helper for password changing via PAM stack"));
    parser.addHelpOption();

    QCommandLineOption userOption(QStringList() << QSL("u") << QSL("user"),
                                  QObject::tr("User name for password changing"),
                                  QSL("user"));
    parser.addOption(userOption);

    parser.process(app);

    if (parser.isSet(userOption))
    {
        user = parser.value(userOption);

        struct passwd *pw = getpwnam(qPrintable(user));
        if (!pw)
        {
            QMessageBox::critical(nullptr,
                                  QSL(PROJECT_NAME),
                                  QObject::tr("User %1 does not exists").arg(user));
            lxqtApp->quit();
            return 1;
        }
        if (getuid() && (pw->pw_uid != getuid()))
        {
            QMessageBox::critical(nullptr,
                                  QSL(PROJECT_NAME),
                                  QObject::tr("You may not view or modify password information for %1").arg(user));
            lxqtApp->quit();
            return 1;
        }
    }
    else
        user = QString::fromLocal8Bit(qgetenv("USER"));

    ChangePasswordDialog dialog;
    dialog.setWindowIcon(QIcon::fromTheme(QSL("preferences-system")));
    dialog.setUser(user);

    bool result = dialog.changePassword();

    if (!result)
    {
        QTextStream(stderr) << QObject::tr("Password does not changed") << endl;
        lxqtApp->quit();
        return 1;
    }

    return 0;
}
