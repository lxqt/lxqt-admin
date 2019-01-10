#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QList>
#include <QWidget>
#include <QProgressDialog>
#include "processworker.h"

class TaskManager : public QObject
{
    Q_OBJECT
public:
    static TaskManager *instance();

    void append(const Worker *worker);
    void run();
    void setWidget(const QWidget *widget);

    enum State
    {
        Processing,
        Finished
    };

signals:
    void stateChanged(TaskManager::State);

private:
    TaskManager();
    ~TaskManager();

    void execFirst();

private slots:
    void onWorkerStateChanged(Worker::WorkerState, QString);

private:
    static TaskManager *mInstance;
    QWidget *mWidget;
    QList<Worker *> mWorkers;
};

#endif // TASKMANAGER_H
