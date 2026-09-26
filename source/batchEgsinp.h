#ifndef BATCHEGSINP_H
#define BATCHEGSINP_H

#include <QtGui>

class Interface;

int runBatchEgsinpPreflight(Interface *ui, const QString &manifestPath, const QString &outputRoot, const QString &reportPath = QString());
int runBatchEgsinpGenerate(Interface *ui, const QString &manifestPath, const QString &outputRoot, const QString &reportPath = QString());

#endif