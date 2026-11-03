#include "ctGeometry.h"

#include <algorithm>
#include <cmath>

namespace {

const double directionTolerance = 1.0e-6;
const double geometryTolerance = 1.0e-5;

bool nearlyEqual(double left, double right, double tolerance = geometryTolerance) {
    double scale = std::max(1.0, std::max(std::abs(left), std::abs(right)));
    return std::abs(left-right) <= tolerance*scale;
}

bool axisDirection(const CTGeometry::Vector &direction, int *axis, int *sign) {
    if (!std::isfinite(direction.x) || !std::isfinite(direction.y) ||
        !std::isfinite(direction.z) || std::abs(direction.z) > directionTolerance) {
        return false;
    }
    if (std::abs(std::abs(direction.x)-1.0) <= directionTolerance &&
        std::abs(direction.y) <= directionTolerance) {
        *axis = 0;
        *sign = direction.x > 0 ? 1 : -1;
        return true;
    }
    if (std::abs(std::abs(direction.y)-1.0) <= directionTolerance &&
        std::abs(direction.x) <= directionTolerance) {
        *axis = 1;
        *sign = direction.y > 0 ? 1 : -1;
        return true;
    }
    return false;
}

QVector<double> inPlaneBoundaries(
    double firstCenterMm,
    int count,
    double spacingMm,
    int sign
) {
    double lastCenterMm = firstCenterMm+sign*(count-1)*spacingMm;
    double minimumCenterMm = std::min(firstCenterMm, lastCenterMm);
    QVector<double> boundaries(count+1);
    for (int i = 0; i <= count; i++)
        boundaries[i] = (minimumCenterMm+(i-0.5)*spacingMm)/10.0;
    return boundaries;
}

}

namespace CTGeometry {

QPoint Mapping::mapPixel(int row, int column) const {
    int x = xFromColumns ? column : row;
    int y = yFromColumns ? column : row;
    if (reverseX)
        x = nx-1-x;
    if (reverseY)
        y = ny-1-y;
    return QPoint(x, y);
}

Result build(const QVector<Slice> &slices, Mapping *mapping, QString *error) {
    if (!mapping || !error || slices.isEmpty()) {
        if (error)
            *error = "CT series is empty.";
        return EmptySeries;
    }

    const Slice &reference = slices.first();
    if (reference.rows <= 0 || reference.columns <= 0) {
        *error = "CT rows and columns must be positive.";
        return InvalidDimensions;
    }
    if (!std::isfinite(reference.rowSpacingMm) ||
        !std::isfinite(reference.columnSpacingMm) ||
        !std::isfinite(reference.positionMm.x) ||
        !std::isfinite(reference.positionMm.y) ||
        !std::isfinite(reference.positionMm.z) ||
        reference.rowSpacingMm <= 0 || reference.columnSpacingMm <= 0) {
        *error = "CT pixel spacing and position must be finite, with positive in-plane spacing.";
        return InvalidSpacing;
    }

    int rowAxis = -1, rowSign = 0, columnAxis = -1, columnSign = 0;
    if (!axisDirection(reference.rowDirection, &rowAxis, &rowSign) ||
        !axisDirection(reference.columnDirection, &columnAxis, &columnSign) ||
        rowAxis == columnAxis) {
        *error = "CT ImageOrientationPatient is not a supported axis-aligned axial orientation.";
        return UnsupportedOrientation;
    }

    for (const Slice &slice : slices) {
        int currentRowAxis = -1, currentRowSign = 0;
        int currentColumnAxis = -1, currentColumnSign = 0;
        if (slice.rows != reference.rows || slice.columns != reference.columns ||
            !nearlyEqual(slice.rowSpacingMm, reference.rowSpacingMm) ||
            !nearlyEqual(slice.columnSpacingMm, reference.columnSpacingMm) ||
            !axisDirection(slice.rowDirection, &currentRowAxis, &currentRowSign) ||
            !axisDirection(slice.columnDirection, &currentColumnAxis, &currentColumnSign) ||
            currentRowAxis != rowAxis || currentRowSign != rowSign ||
            currentColumnAxis != columnAxis || currentColumnSign != columnSign ||
            !nearlyEqual(slice.positionMm.x, reference.positionMm.x) ||
            !nearlyEqual(slice.positionMm.y, reference.positionMm.y) ||
            !std::isfinite(slice.positionMm.z)) {
            *error = "CT slices have inconsistent dimensions, spacing, orientation, or in-plane position.";
            return InconsistentGeometry;
        }
    }

    mapping->xFromColumns = rowAxis == 0;
    mapping->yFromColumns = rowAxis == 1;
    mapping->nx = mapping->xFromColumns ? reference.columns : reference.rows;
    mapping->ny = mapping->yFromColumns ? reference.columns : reference.rows;
    mapping->nz = slices.size();
    int xSign = rowAxis == 0 ? rowSign : columnSign;
    int ySign = rowAxis == 1 ? rowSign : columnSign;
    double xSpacingMm = rowAxis == 0 ? reference.columnSpacingMm : reference.rowSpacingMm;
    double ySpacingMm = rowAxis == 1 ? reference.columnSpacingMm : reference.rowSpacingMm;
    mapping->reverseX = xSign < 0;
    mapping->reverseY = ySign < 0;
    mapping->xBoundariesCm = inPlaneBoundaries(
        reference.positionMm.x, mapping->nx, xSpacingMm, xSign
    );
    mapping->yBoundariesCm = inPlaneBoundaries(
        reference.positionMm.y, mapping->ny, ySpacingMm, ySign
    );

    mapping->sliceOrder.resize(slices.size());
    for (int i = 0; i < slices.size(); i++)
        mapping->sliceOrder[i] = i;
    std::sort(
        mapping->sliceOrder.begin(),
        mapping->sliceOrder.end(),
        [&slices](int left, int right) {
            return slices[left].positionMm.z < slices[right].positionMm.z;
        }
    );

    mapping->zBoundariesCm.resize(slices.size()+1);
    if (slices.size() == 1) {
        if (!std::isfinite(reference.sliceThicknessMm) || reference.sliceThicknessMm <= 0) {
            *error = "A single-slice CT requires a finite positive SliceThickness.";
            return InvalidSpacing;
        }
        mapping->zBoundariesCm[0] =
            (reference.positionMm.z-reference.sliceThicknessMm/2.0)/10.0;
        mapping->zBoundariesCm[1] =
            (reference.positionMm.z+reference.sliceThicknessMm/2.0)/10.0;
        return Success;
    }

    QVector<double> centersMm(slices.size());
    for (int i = 0; i < mapping->sliceOrder.size(); i++)
        centersMm[i] = slices[mapping->sliceOrder[i]].positionMm.z;
    for (int i = 1; i < centersMm.size(); i++) {
        if (nearlyEqual(centersMm[i], centersMm[i-1])) {
            *error = "CT slices contain duplicate physical positions.";
            return DuplicateSlicePosition;
        }
    }
    mapping->zBoundariesCm[0] = (centersMm[0]-(centersMm[1]-centersMm[0])/2.0)/10.0;
    for (int i = 1; i < centersMm.size(); i++)
        mapping->zBoundariesCm[i] = (centersMm[i-1]+centersMm[i])/20.0;
    mapping->zBoundariesCm.last() =
        (centersMm.last()+(centersMm.last()-centersMm[centersMm.size()-2])/2.0)/10.0;
    return Success;
}

}