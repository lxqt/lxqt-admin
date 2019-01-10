#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <LXQt/SingleApplication>
#include "misc.h"
#include "taskmanager.h"

bool pkexec(const QStringList& command, const QByteArray& stdinData) {
    Q_ASSERT(!command.isEmpty());
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

    while (!process.waitForFinished(-1))
        LXQt::Application::processEvents();

    QByteArray pkexec_error = process.readAllStandardError();
    qDebug() << pkexec_error;
    const bool succeeded = process.exitCode() == 0;
    if (!succeeded)
    {
        QMessageBox * msg = new QMessageBox{QMessageBox::Critical, QObject::tr("Error")
            , QObject::tr("<strong>Action (%1) failed:</strong><br/><pre>%2</pre>").arg(command[0]).arg(QString::fromUtf8(pkexec_error.constData()))};
        msg->setAttribute(Qt::WA_DeleteOnClose, true);
        msg->show();
    }
    return succeeded;
}

bool pkexecGetOut(const QStringList &command, QString *out)
{
    Q_ASSERT(!command.isEmpty());
    QProcess process;
    qDebug() << command;
    QStringList args;
    args << QStringLiteral("--disable-internal-agent")
        << QStringLiteral("lxqt-admin-user-helper")
        << command;
    process.start(QStringLiteral("pkexec"), args);
    process.waitForFinished(-1);
    if (out)
        *out = QString::fromUtf8(process.readAllStandardOutput());
    QByteArray pkexec_error = process.readAllStandardError();
    const bool succeeded = process.exitCode() == 0;
    if (!succeeded)
    {
        QMessageBox * msg = new QMessageBox{QMessageBox::Critical, QObject::tr("Error")
            , QObject::tr("<strong>Action (%1) failed:</strong><br/><pre>%2</pre>").arg(command[0]).arg(QString::fromUtf8(pkexec_error.constData()))};
        msg->setAttribute(Qt::WA_DeleteOnClose, true);
        msg->show();
    }
    return succeeded;
}
