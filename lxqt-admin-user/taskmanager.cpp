#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QProgressBar>
#include <QApplication>
#include "taskmanager.h"

TaskManager::TaskManager()
{}

TaskManager::~TaskManager()
{}

void TaskManager::execFirst()
{
    if (mWorkers.size())
    {
        Worker *worker = mWorkers.first();
        mWorkers.pop_front();

        QThread *thread = new QThread();
        worker->moveToThread(thread);

        connect(thread, SIGNAL(started()), worker, SLOT(onRun()));
        connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
        connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        connect(worker, SIGNAL(notify(Worker::WorkerState, QString)), this, SLOT(onWorkerStateChanged(Worker::WorkerState, QString)));
        thread->start();
    }
    else
        Q_EMIT stateChanged(TaskManager::Finished);
}

void TaskManager::onWorkerStateChanged(Worker::WorkerState state, QString msg)
{
    switch (state)
    {
        case ProcessWorker::Finished:
        {
            execFirst();
        }
        break;
        case ProcessWorker::FinishedWithError:
        {
            mWorkers.clear();
            QMessageBox::critical(mWidget,
                                  QObject::tr("Error"),
                                  msg);
            Q_EMIT stateChanged(TaskManager::Finished);
        }
        break;
        default:
            break;
    }
}

void TaskManager::run()
{
    if (mWorkers.size() == 0)
        return;
    Q_EMIT stateChanged(TaskManager::Processing);
    execFirst();
}

void TaskManager::setWidget(const QWidget *widget)
{
    mWidget = (QWidget *) widget;
}

TaskManager *TaskManager::instance()
{
    if (mInstance == nullptr)
        mInstance = new TaskManager;
    return mInstance;
}

void TaskManager::append(const Worker *worker)
{
    mWorkers.append((Worker *) worker);
}

TaskManager* TaskManager::mInstance = nullptr;
