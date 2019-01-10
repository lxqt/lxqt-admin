#ifndef MISC_H
#define MISC_H

#include <QStringList>

bool pkexec(const QStringList& command, const QByteArray& stdinData = QByteArray());
bool pkexecGetOut(const QStringList& command, QString *out);


#endif // MISC_H
