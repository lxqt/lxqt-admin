/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *   Palo Kisa <palo.kisa@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <LXQt/Application>
#include "passworddialog.h"
#include "communication.h"
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <iostream>
#include <signal.h>
#include <QDebug>

const QString app_master{QStringLiteral(LXQTSUDO)};
const QString install_dir{QStringLiteral(LXQTSUDO_INSTALL_DIR)};
const QString app_slave{QStringLiteral(LXQTSUDO_HELPER)};
#define ENV_SUDO_ASKPASS "SUDO_ASKPASS"
#define ENV_KEY "LXQTSUDO_KEY"

void termSignalHandler(int)
{
    if (QApplication::instance())
        QApplication::instance()->quit();
}

int master(int argc, char **argv)
{
    //master
    LxQt::Application app(argc, argv);
    QString key = QStringLiteral("%1/%2").arg(app_master).arg(app.applicationPid());
    Communication comm(key, true/*master*/);
    if (!comm.valid())
        return 1;

    //set environment for child
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(ENV_KEY, key);
    QFileInfo fi(argv[0]);
    if (nullptr != strchr(argv[0], '/'))
        fi.setFile(fi.dir(), app_slave);
    else
        fi.setFile(QDir(install_dir), app_slave);
    env.insert(ENV_SUDO_ASKPASS, fi.filePath());

    //start background process -> sudo
    QStringList args = app.arguments();
    //XXX: check?!? if there is something to run args.size() > 1
    args.removeAt(0);
    QProcess * sudo = new QProcess(&app);
    sudo->setProcessEnvironment(env);
    sudo->setInputChannelMode(QProcess::ForwardedInputChannel);
    sudo->setReadChannelMode(QProcess::ForwardedOutputChannel);
    sudo->setReadChannel(QProcess::StandardError);
    sudo->start(QStringLiteral(LXQTSUDO_SUDO), QStringList() << QStringLiteral("-A")
            << QStringLiteral("-b")
            << args);

    PasswordDialog dlg(args, comm, *sudo);
    dlg.setWindowIcon(QIcon::fromTheme("security-high"));
    app.setActiveWindow(&dlg);
    dlg.show();
    return app.exec();
}

int slave(int argc, char **argv)
{
    //slave
    QApplication app(argc, argv);
    QByteArray key = qgetenv(ENV_KEY);
    Q_ASSERT(!key.isEmpty());
    Communication comm(key.constData(), false/*slave*/);
    if (!comm.valid())
        return 1;

    QObject::connect(&comm, &Communication::passwordReady, [&app] (QString passwd)
        {
            std::cout << passwd.toUtf8().constData() << std::endl;
            app.quit();
        });
    comm.needPassword();
    comm.waitForReady();
    return app.exec();
}

int main(int argc, char **argv)
{
    QFileInfo fi(argv[0]); //first argument (executable) is there always

    // Quit gracefully
    ::signal(SIGTERM, termSignalHandler);
    ::signal(SIGINT,  termSignalHandler);
    ::signal(SIGHUP,  termSignalHandler);

    if (app_master == fi.fileName())
        return master(argc, argv);
    else
        return slave(argc, argv);
}
