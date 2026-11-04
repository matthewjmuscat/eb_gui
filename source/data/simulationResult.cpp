#include "simulationResult.h"

#include <QDir>
#include <QFileInfo>

namespace SimulationResult {

QStringList findDosePaths(
    const QString &runDirectory,
    const QString &simulationName
) {
    QStringList dosePaths;
    QDir directory(runDirectory);
    QFileInfoList doseFiles = directory.entryInfoList(
        {"*.3ddose", "*.3ddose.gz"},
        QDir::Files | QDir::Readable,
        QDir::Name
    );
    for (const QFileInfo &doseFile : doseFiles)
        if (doseFile.fileName().startsWith(simulationName+"."))
            dosePaths << doseFile.absoluteFilePath();
    return dosePaths;
}

Evaluation evaluate(
    const QString &runDirectory,
    const QString &simulationName,
    int exitCode,
    QProcess::ExitStatus exitStatus
) {
    Evaluation result;
    if (exitStatus != QProcess::NormalExit) {
        result.error = "egs_brachy terminated abnormally.";
        return result;
    }
    if (exitCode != 0) {
        result.error = "egs_brachy exited with code "+QString::number(exitCode)+".";
        return result;
    }

    for (const QString &dosePath : findDosePaths(runDirectory, simulationName))
        if (QFileInfo(dosePath).size() > 0)
            result.dosePaths << dosePath;

    if (result.dosePaths.isEmpty()) {
        result.error = "egs_brachy produced no nonempty dose output.";
        return result;
    }

    result.successful = true;
    return result;
}

}