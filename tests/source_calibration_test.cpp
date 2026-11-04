#include "../source/data/sourceCalibration.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include <cmath>
#include <iostream>
#include <limits>

namespace {

bool writeText(const QString &path, const QString &text) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream output(&file);
    output << text;
    return true;
}

bool closeTo(double left, double right, double tolerance = 1.0e-12) {
    return std::abs(left-right) <= tolerance;
}

}

int main(int argc, char *argv[]) {
    QCoreApplication application(argc, argv);
    QString root = QCoreApplication::applicationDirPath()+"/synthetic_source_calibration";
    QDir().mkpath(root);
    QString geometryPath = root+"/source.geom";
    QString error;
    double scale = -1.0;

    if (!writeText(geometryPath, "# air kerma: 2.0\n") ||
        !SourceCalibration::calculateDoseScale(
            geometryPath, 4.0, 3.0/3600.0, &scale, &error
        ) ||
        !closeTo(scale, 4.0/2.0*3.0/3600.0)) {
        std::cerr << "Valid source calibration produced an incorrect scale\n";
        return 1;
    }

    const QStringList invalidGeometry = {
        "source geometry without calibration\n",
        "# air kerma: 0\n",
        "# air kerma: nan\n",
        "# air kerma: invalid\n"
    };
    for (const QString &content : invalidGeometry) {
        if (!writeText(geometryPath, content))
            return 1;
        scale = -1.0;
        if (SourceCalibration::calculateDoseScale(
                geometryPath, 4.0, 3.0/3600.0, &scale, &error
            ) || scale != -1.0) {
            std::cerr << "Invalid source calibration was accepted\n";
            return 1;
        }
    }

    if (SourceCalibration::calculateDoseScale(
            root+"/missing.geom", 4.0, 3.0/3600.0, &scale, &error
        ) ||
        SourceCalibration::calculateDoseScale(
            geometryPath, 0.0, 3.0/3600.0, &scale, &error
        ) ||
        SourceCalibration::calculateDoseScale(
            geometryPath, 4.0, std::numeric_limits<double>::quiet_NaN(), &scale, &error
        )) {
        std::cerr << "Invalid calibration inputs were accepted\n";
        return 1;
    }

    std::cout << "Passed source calibration test\n";
    return 0;
}