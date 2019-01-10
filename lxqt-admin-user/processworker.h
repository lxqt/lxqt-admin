#ifndef PROCESSWORKER_H
#define PROCESSWORKER_H

#include <QObject>
#include <QProcess>


class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(QObject *parent = 0);
    ~Worker();

    enum WorkerState
    {
        Undefined,
        Started,
        Finished,
        FinishedWithError
    };
public slots:
    virtual void onRun() = 0;

signals:
    void notify(Worker::WorkerState state, QString msg = QString());
    void finished();

protected:
    WorkerState mState;
};


/* ********************************************************************************* */

class ProcessWorker : public Worker
{
    Q_OBJECT
public:
    ProcessWorker(const QString &cmd, const QStringList &args, QObject *parent = 0);
    ~ProcessWorker();

public slots:
    virtual void onRun();

private:
    QString mCmd;
    QStringList mArgs;
    QProcess *mProcess;
    QStringList mResult;
};

#endif // PROCESSWORKER_H
