#include "sourceCalibration.h"

#include <QFile>
#include <QRegExp>

#include <cmath>

namespace SourceCalibration {

bool calculateDoseScale(
    const QString &sourceGeometryPath,
    double clinicalAirKermaStrength,
    double exposureHours,
    double *doseScale,
    QString *error
) {
    if (!doseScale || !error)
        return false;
    if (!std::isfinite(clinicalAirKermaStrength) || clinicalAirKermaStrength <= 0.0) {
        *error = "Clinical air-kerma strength must be finite and positive.";
        return false;
    }
    if (!std::isfinite(exposureHours) || exposureHours <= 0.0) {
        *error = "Treatment exposure time must be finite and positive.";
        return false;
    }

    QFile sourceGeometry(sourceGeometryPath);
    if (!sourceGeometry.open(QIODevice::ReadOnly | QIODevice::Text)) {
        *error = "Could not open the selected source geometry file.";
        return false;
    }

    QRegExp calibrationLine(
        "^\\s*#\\s*air\\s+kerma\\s*:\\s*(.+)\\s*$",
        Qt::CaseInsensitive
    );
    while (!sourceGeometry.atEnd()) {
        QString line = QString::fromUtf8(sourceGeometry.readLine());
        if (calibrationLine.indexIn(line) != 0)
            continue;

        QString valueText = calibrationLine.cap(1).split("#").first().trimmed();
        bool ok = false;
        double modelAirKermaPerHistory = valueText.toDouble(&ok);
        if (!ok || !std::isfinite(modelAirKermaPerHistory) || modelAirKermaPerHistory <= 0.0) {
            *error = "Source-model air-kerma calibration must be finite and positive.";
            return false;
        }

        double calculatedScale = clinicalAirKermaStrength/modelAirKermaPerHistory*exposureHours;
        if (!std::isfinite(calculatedScale) || calculatedScale <= 0.0) {
            *error = "Calculated dose scaling factor is invalid.";
            return false;
        }
        *doseScale = calculatedScale;
        error->clear();
        return true;
    }

    *error = "Selected source geometry has no air-kerma-per-history calibration.";
    return false;
}

}