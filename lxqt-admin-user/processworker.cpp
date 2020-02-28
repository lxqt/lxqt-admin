#include <QDebug>
#include "processworker.h"


Worker::Worker(QObject *parent)
: QObject(parent)
{
    qRegisterMetaType<Worker::WorkerState>("Worker::WorkerState");
}

Worker::~Worker()
{}

/* *************************************************************************************** */

ProcessWorker::ProcessWorker(const QString &cmd, const QStringList &args, QObject *parent)
: Worker(parent),
  mCmd(cmd),
  mArgs(args),
  mProcess(new QProcess(this))
{
    mProcess->setProcessChannelMode(QProcess::MergedChannels);

    connect(mProcess,
             static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
             [=] {
        if (mProcess->exitCode() || (mProcess->exitStatus() != QProcess::NormalExit))
            mState = FinishedWithError;
        else
            mState = Finished;
        qDebug() << Q_FUNC_INFO << "cmd: " << cmd << " " << args << " ret: " << mState;
        Q_EMIT notify(mState, QString::fromUtf8(mProcess->readAll()));
        Q_EMIT finished();
    } );
}

ProcessWorker::~ProcessWorker()
{}

void ProcessWorker::onRun()
{
    mProcess->start(mCmd, mArgs);
    mState = Started;
     Q_EMIT notify(mState);
}
