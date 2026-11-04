#ifndef SIMULATION_RESULT_H
#define SIMULATION_RESULT_H

#include <QProcess>
#include <QString>
#include <QStringList>

namespace SimulationResult {

struct Evaluation {
    bool successful = false;
    QString error;
    QStringList dosePaths;
};

QStringList findDosePaths(
    const QString &runDirectory,
    const QString &simulationName
);

Evaluation evaluate(
    const QString &runDirectory,
    const QString &simulationName,
    int exitCode,
    QProcess::ExitStatus exitStatus
);

}

#endif