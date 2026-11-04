#ifndef SOURCE_CALIBRATION_H
#define SOURCE_CALIBRATION_H

#include <QString>

namespace SourceCalibration {

bool calculateDoseScale(
    const QString &sourceGeometryPath,
    double clinicalAirKermaStrength,
    double exposureHours,
    double *doseScale,
    QString *error
);

}

#endif