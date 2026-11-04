#include "../source/data/simulationResult.h"

#include <QCoreApplication>
#include <QFile>
#include <QTemporaryDir>

#include <iostream>

namespace {

bool writeBytes(const QString &path, const QByteArray &bytes) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    return file.write(bytes) == bytes.size();
}

}

int main(int argc, char *argv[]) {
    QCoreApplication application(argc, argv);
    QTemporaryDir runDirectory;
    if (!runDirectory.isValid())
        return 1;

    const QString simulationName = "synthetic_run";
    const QString dosePath = runDirectory.path()+"/"+simulationName+".phantom.3ddose";

    if (SimulationResult::evaluate(
            runDirectory.path(), simulationName, 2, QProcess::NormalExit
        ).successful ||
        SimulationResult::evaluate(
            runDirectory.path(), simulationName, 0, QProcess::CrashExit
        ).successful ||
        SimulationResult::evaluate(
            runDirectory.path(), simulationName, 0, QProcess::NormalExit
        ).successful) {
        std::cerr << "Failed process or missing output was accepted\n";
        return 1;
    }

    if (!writeBytes(dosePath, QByteArray()) ||
        SimulationResult::findDosePaths(
            runDirectory.path(), simulationName
        ) != QStringList(dosePath) ||
        SimulationResult::evaluate(
            runDirectory.path(), simulationName, 0, QProcess::NormalExit
        ).successful) {
        std::cerr << "Empty dose output was accepted\n";
        return 1;
    }

    if (!writeBytes(dosePath, "synthetic dose") )
        return 1;
    SimulationResult::Evaluation success = SimulationResult::evaluate(
        runDirectory.path(), simulationName, 0, QProcess::NormalExit
    );
    if (!success.successful || success.dosePaths != QStringList(dosePath)) {
        std::cerr << "Clean process with dose output was rejected\n";
        return 1;
    }

    std::cout << "Passed simulation result test\n";
    return 0;
}