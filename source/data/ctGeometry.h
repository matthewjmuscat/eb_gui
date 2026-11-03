#ifndef CT_GEOMETRY_H
#define CT_GEOMETRY_H

#include <QPoint>
#include <QString>
#include <QVector>

namespace CTGeometry {

enum Result {
    Success = 0,
    EmptySeries,
    InvalidDimensions,
    InvalidSpacing,
    UnsupportedOrientation,
    InconsistentGeometry,
    DuplicateSlicePosition
};

struct Vector {
    double x;
    double y;
    double z;

    Vector(double xValue = 0.0, double yValue = 0.0, double zValue = 0.0) :
        x(xValue), y(yValue), z(zValue) {}
};

struct Slice {
    int rows = 0;
    int columns = 0;
    double rowSpacingMm = 0.0;
    double columnSpacingMm = 0.0;
    double sliceThicknessMm = 0.0;
    Vector positionMm;
    Vector rowDirection;
    Vector columnDirection;
};

struct Mapping {
    int nx = 0;
    int ny = 0;
    int nz = 0;
    bool xFromColumns = true;
    bool yFromColumns = false;
    bool reverseX = false;
    bool reverseY = false;
    QVector<int> sliceOrder;
    QVector<double> xBoundariesCm;
    QVector<double> yBoundariesCm;
    QVector<double> zBoundariesCm;

    QPoint mapPixel(int row, int column) const;
};

Result build(const QVector<Slice> &slices, Mapping *mapping, QString *error);

}

#endif