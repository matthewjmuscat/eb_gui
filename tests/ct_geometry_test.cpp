#include "../source/data.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QTextStream>

#include <cmath>
#include <cstring>
#include <iostream>

namespace {

Attribute *textAttribute(quint16 group, quint16 element, const QString &value) {
    QByteArray bytes = value.toLatin1();
    Attribute *attribute = new Attribute();
    attribute->tag[0] = group;
    attribute->tag[1] = element;
    attribute->vl = static_cast<unsigned long>(bytes.size());
    attribute->vf = new unsigned char[attribute->vl];
    if (attribute->vl)
        std::memcpy(attribute->vf, bytes.constData(), attribute->vl);
    return attribute;
}

Attribute *unsignedShortAttribute(quint16 group, quint16 element, quint16 value) {
    Attribute *attribute = new Attribute();
    attribute->tag[0] = group;
    attribute->tag[1] = element;
    attribute->vl = 2;
    attribute->vf = new unsigned char[2];
    attribute->vf[0] = static_cast<unsigned char>(value & 0xff);
    attribute->vf[1] = static_cast<unsigned char>((value >> 8) & 0xff);
    return attribute;
}

Attribute *pixelAttribute(const QVector<qint16> &values) {
    Attribute *attribute = new Attribute();
    attribute->tag[0] = 0x7fe0;
    attribute->tag[1] = 0x0010;
    attribute->vl = static_cast<unsigned long>(values.size()*2);
    attribute->vf = new unsigned char[attribute->vl];
    for (int i = 0; i < values.size(); i++) {
        quint16 raw = static_cast<quint16>(values[i]);
        attribute->vf[2*i] = static_cast<unsigned char>(raw & 0xff);
        attribute->vf[2*i+1] = static_cast<unsigned char>((raw >> 8) & 0xff);
    }
    return attribute;
}

DICOM *slice(
    Data *data,
    int rows,
    int columns,
    const QString &position,
    const QString &orientation,
    const QString &spacing,
    const QVector<qint16> &pixels,
    const QString &thickness = "3"
) {
    DICOM *dicom = new DICOM(&data->tag_data);
    dicom->isBigEndian = false;
    dicom->data.append(textAttribute(0x0018, 0x0050, thickness));
    dicom->data.append(textAttribute(0x0020, 0x0032, position));
    if (!orientation.isNull())
        dicom->data.append(textAttribute(0x0020, 0x0037, orientation));
    dicom->data.append(unsignedShortAttribute(0x0028, 0x0010, rows));
    dicom->data.append(unsignedShortAttribute(0x0028, 0x0011, columns));
    dicom->data.append(textAttribute(0x0028, 0x0030, spacing));
    dicom->data.append(textAttribute(0x0028, 0x1052, "0"));
    dicom->data.append(textAttribute(0x0028, 0x1053, "1"));
    dicom->data.append(pixelAttribute(pixels));
    return dicom;
}

bool closeTo(double left, double right, double tolerance = 1.0e-9) {
    return std::abs(left-right) <= tolerance;
}

void configureData(Data *data, const QString &calibrationPath) {
    data->hu_location = calibrationPath;
    data->TAS_names << "Synthetic";
    data->media.resize(1);
    data->media[0] << "LOW" << "HIGH";
    data->threshold.resize(1);
    data->threshold[0] << 0.45 << 10.0;
}

void clearSlices(Data *data) {
    for (DICOM *dicom : data->CT_data)
        delete dicom;
    data->CT_data.clear();
}

int build(
    Data *data,
    EGSPhant *phantom,
    int contourCount = 0,
    QVector<EGSPhant *> *masks = nullptr,
    QString *buildLog = nullptr
) {
    QVector<int> structureIndices;
    QVector<int> tissueIndices;
    QVector<EGSPhant *> localMasks;
    if (!masks)
        masks = &localMasks;
    for (int i = 0; i < contourCount; i++) {
        structureIndices << i;
        tissueIndices << -1;
    }
    QString localLog;
    if (!buildLog)
        buildLog = &localLog;
    phantom->maxDensity = 0;
    int result = data->buildEgsphant(
        phantom, buildLog, contourCount, 0, &structureIndices, &tissueIndices, masks
    );
    for (EGSPhant *mask : localMasks)
        delete mask;
    return result;
}

bool testConventional(const QString &calibrationPath) {
    Data data;
    configureData(&data, calibrationPath);
    data.CT_data << slice(
        &data, 2, 3, "10\\20\\0", "1\\0\\0\\0\\1\\0", "2\\1",
        {10, 20, 30, 40, 50, 60}
    );
    EGSPhant phantom;
    int result = build(&data, &phantom);
    bool passed = result == 0 && phantom.nx == 3 && phantom.ny == 2 &&
        closeTo(phantom.x[0], 0.95) && closeTo(phantom.x[3], 1.25) &&
        closeTo(phantom.y[0], 1.9) && closeTo(phantom.y[2], 2.3) &&
        closeTo(phantom.d[0][0][0], 0.2) && closeTo(phantom.d[2][1][0], 0.7) &&
        phantom.m[0][0][0] == '1' && phantom.m[2][1][0] == '2';
    clearSlices(&data);
    return passed;
}

bool testReversedXMaskAndSource(const QString &calibrationPath) {
    Data data;
    configureData(&data, calibrationPath);
    data.CT_data << slice(
        &data, 2, 3, "10\\20\\0", "-1\\0\\0\\0\\1\\0", "2\\1",
        {10, 20, 30, 40, 50, 60}
    );
    data.structName << "Target";
    data.structPos.resize(1);
    data.structZ.resize(1);
    QPolygonF contour;
    contour << QPointF(0.76, 1.91) << QPointF(0.84, 1.91)
            << QPointF(0.84, 2.09) << QPointF(0.76, 2.09);
    data.structPos[0] << contour;
    data.structZ[0] << 0.0;

    QString root = QFileInfo(calibrationPath).absolutePath();
    QDir().mkpath(root+"/database/transformation");
    data.gui_location = root;
    data.do_MAR = true;
    data.marContourInd = -1;
    data.marRad = 0.01;
    data.lowerThresh = 0.0;
    data.upperThresh = 100.0;
    data.marDen = 1.0;
    data.transformFile = "synthetic_source";
    QFile transformation(root+"/database/transformation/synthetic_source");
    if (!transformation.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream transformationOutput(&transformation);
    transformationOutput << "translation = 0.8 2.0 0.0\n";
    transformation.close();

    EGSPhant phantom;
    QVector<EGSPhant *> masks;
    QString log;
    int result = build(&data, &phantom, 1, &masks, &log);
    bool passed = result == 0 && phantom.nx == 3 && phantom.ny == 2 &&
        closeTo(phantom.x[0], 0.75) && closeTo(phantom.x[3], 1.05) &&
        closeTo(phantom.y[0], 1.9) && closeTo(phantom.y[2], 2.3) &&
        closeTo(phantom.d[0][0][0], 0.4) && closeTo(phantom.d[2][0][0], 0.2) &&
        closeTo(phantom.d[0][1][0], 0.7) && closeTo(phantom.d[2][1][0], 0.5) &&
        masks.size() == 1 && masks[0]->m[0][0][0] == 50 &&
        masks[0]->m[1][0][0] == 49 &&
        phantom.getIndex("x axis", 0.8) == 0 &&
        phantom.getIndex("y axis", 2.0) == 0 &&
        phantom.getIndex("z axis", 0.0) == 0 &&
        phantom.getIndex("x axis", 1.1) == -1 &&
        log.contains("MAR performed at source position") &&
        log.contains("of the 1 evaluated voxels");
    for (EGSPhant *mask : masks)
        delete mask;
    clearSlices(&data);
    return passed;
}

bool testReversedY(const QString &calibrationPath) {
    Data data;
    configureData(&data, calibrationPath);
    data.CT_data << slice(
        &data, 2, 3, "10\\20\\0", "1\\0\\0\\0\\-1\\0", "2\\1",
        {10, 20, 30, 40, 50, 60}
    );
    EGSPhant phantom;
    int result = build(&data, &phantom);
    bool passed = result == 0 &&
        closeTo(phantom.y[0], 1.7) && closeTo(phantom.y[2], 2.1) &&
        closeTo(phantom.d[0][0][0], 0.5) && closeTo(phantom.d[0][1][0], 0.2);
    clearSlices(&data);
    return passed;
}

bool testSwappedAxes(const QString &calibrationPath) {
    Data data;
    configureData(&data, calibrationPath);
    data.CT_data << slice(
        &data, 2, 3, "10\\20\\0", "0\\1\\0\\1\\0\\0", "2\\1",
        {10, 20, 30, 40, 50, 60}
    );
    EGSPhant phantom;
    int result = build(&data, &phantom);
    bool passed = result == 0 && phantom.nx == 2 && phantom.ny == 3 &&
        closeTo(phantom.x[0], 0.9) && closeTo(phantom.x[2], 1.3) &&
        closeTo(phantom.y[0], 1.95) && closeTo(phantom.y[3], 2.25) &&
        closeTo(phantom.d[0][0][0], 0.2) && closeTo(phantom.d[1][2][0], 0.7);
    clearSlices(&data);
    return passed;
}

bool testRotatedAxes(const QString &calibrationPath) {
    Data data;
    configureData(&data, calibrationPath);
    data.CT_data << slice(
        &data, 2, 3, "10\\20\\0", "0\\1\\0\\-1\\0\\0", "2\\1",
        {10, 20, 30, 40, 50, 60}
    );
    EGSPhant phantom;
    int result = build(&data, &phantom);
    bool passed = result == 0 && phantom.nx == 2 && phantom.ny == 3 &&
        closeTo(phantom.x[0], 0.7) && closeTo(phantom.x[2], 1.1) &&
        closeTo(phantom.y[0], 1.95) && closeTo(phantom.y[3], 2.25) &&
        closeTo(phantom.d[0][0][0], 0.5) && closeTo(phantom.d[1][2][0], 0.4);
    clearSlices(&data);
    return passed;
}

bool testDescendingSliceInput(const QString &calibrationPath) {
    Data data;
    configureData(&data, calibrationPath);
    data.CT_data << slice(&data, 1, 1, "0\\0\\3", "1\\0\\0\\0\\1\\0", "1\\1", {60});
    data.CT_data << slice(&data, 1, 1, "0\\0\\0", "1\\0\\0\\0\\1\\0", "1\\1", {10});
    EGSPhant phantom;
    int result = build(&data, &phantom);
    bool passed = result == 0 && phantom.nz == 2 &&
        closeTo(phantom.z[0], -0.15) && closeTo(phantom.z[1], 0.15) &&
        closeTo(phantom.z[2], 0.45) &&
        closeTo(phantom.d[0][0][0], 0.2) && closeTo(phantom.d[0][0][1], 0.7);
    clearSlices(&data);
    return passed;
}

bool testNonuniformSliceSpacing(const QString &calibrationPath) {
    Data data;
    configureData(&data, calibrationPath);
    data.CT_data << slice(&data, 1, 1, "0\\0\\0", "1\\0\\0\\0\\1\\0", "1\\1", {10}, "");
    data.CT_data << slice(&data, 1, 1, "0\\0\\2", "1\\0\\0\\0\\1\\0", "1\\1", {20}, "");
    data.CT_data << slice(&data, 1, 1, "0\\0\\7", "1\\0\\0\\0\\1\\0", "1\\1", {60}, "");
    EGSPhant phantom;
    int result = build(&data, &phantom);
    bool passed = result == 0 && phantom.nz == 3 &&
        closeTo(phantom.z[0], -0.1) && closeTo(phantom.z[1], 0.1) &&
        closeTo(phantom.z[2], 0.45) && closeTo(phantom.z[3], 0.95) &&
        closeTo(phantom.d[0][0][0], 0.2) &&
        closeTo(phantom.d[0][0][1], 0.3) &&
        closeTo(phantom.d[0][0][2], 0.7);
    clearSlices(&data);
    return passed;
}

bool testEmptyThicknessUniformSeries(const QString &calibrationPath) {
    Data data;
    configureData(&data, calibrationPath);
    data.CT_data << slice(&data, 1, 1, "0\\0\\0", "1\\0\\0\\0\\1\\0", "1\\1", {10}, "");
    data.CT_data << slice(&data, 1, 1, "0\\0\\3", "1\\0\\0\\0\\1\\0", "1\\1", {20}, "");
    EGSPhant phantom;
    int result = build(&data, &phantom);
    bool passed = result == 0 && phantom.nz == 2 &&
        closeTo(phantom.z[0], -0.15) && closeTo(phantom.z[1], 0.15) &&
        closeTo(phantom.z[2], 0.45) &&
        closeTo(phantom.d[0][0][0], 0.2) && closeTo(phantom.d[0][0][1], 0.3);
    clearSlices(&data);
    return passed;
}

bool testEmptyThicknessSingleSlice(const QString &calibrationPath) {
    Data data;
    configureData(&data, calibrationPath);
    data.CT_data << slice(
        &data, 1, 1, "0\\0\\0", "1\\0\\0\\0\\1\\0", "1\\1", {10}, ""
    );
    EGSPhant phantom;
    int result = build(&data, &phantom);
    clearSlices(&data);
    return result == 209;
}

bool testEmptySeries(const QString &calibrationPath) {
    Data data;
    configureData(&data, calibrationPath);
    EGSPhant phantom;
    return build(&data, &phantom) == 209;
}

bool testRejectedGeometry(const QString &calibrationPath) {
    const struct {
        QString firstPosition;
        QString firstOrientation;
        QString firstSpacing;
        QString secondPosition;
        QString secondOrientation;
        QString secondSpacing;
        int expected;
    } cases[] = {
        {"0\\0\\0", "0.7071068\\0.7071068\\0\\-0.7071068\\0.7071068\\0", "1\\1", QString(), QString(), QString(), 209},
        {"0\\0\\0", "1\\0\\0\\0\\1\\0", "1\\1", "0\\0\\3", "-1\\0\\0\\0\\1\\0", "1\\1", 209},
        {"0\\0\\0", "1\\0\\0\\0\\1\\0", "1\\1", "1\\0\\3", "1\\0\\0\\0\\1\\0", "1\\1", 209},
        {"0\\0\\0", "1\\0\\0\\0\\1\\0", "1\\1", "0\\0\\0", "1\\0\\0\\0\\1\\0", "1\\1", 209},
        {"0\\0\\0", "1\\0\\0\\0\\1\\0", "1\\1", "0\\0\\3", "1\\0\\0\\0\\1\\0", "2\\1", 209},
        {"0\\0\\0", "1\\0\\0\\0\\1\\0", "bad\\1", QString(), QString(), QString(), 209},
        {"0\\0\\0", QString(), "1\\1", QString(), QString(), QString(), 206},
    };
    for (const auto &testCase : cases) {
        Data data;
        configureData(&data, calibrationPath);
        data.CT_data << slice(
            &data, 1, 1, testCase.firstPosition, testCase.firstOrientation,
            testCase.firstSpacing, {10}
        );
        if (!testCase.secondPosition.isNull())
            data.CT_data << slice(
                &data, 1, 1, testCase.secondPosition, testCase.secondOrientation,
                testCase.secondSpacing, {20}
            );
        EGSPhant phantom;
        int result = build(&data, &phantom);
        clearSlices(&data);
        if (result != testCase.expected)
            return false;
    }
    return true;
}

}

int main(int argc, char *argv[]) {
    QCoreApplication application(argc, argv);
    QTemporaryDir temporary;
    if (!temporary.isValid())
        return 1;

    QString calibrationPath = temporary.filePath("synthetic.hu2rho");
    QFile calibration(calibrationPath);
    if (!calibration.open(QIODevice::WriteOnly | QIODevice::Text))
        return 1;
    QTextStream calibrationOutput(&calibration);
    calibrationOutput << "0 0.1\n100 1.1\n";
    calibration.close();

    const struct {
        const char *name;
        bool (*run)(const QString &);
    } tests[] = {
        {"conventional unequal spacing", testConventional},
        {"reversed x with contour/source checks", testReversedXMaskAndSource},
        {"reversed y", testReversedY},
        {"swapped axes", testSwappedAxes},
        {"rotated axes", testRotatedAxes},
        {"descending slice input", testDescendingSliceInput},
        {"empty thickness uniform series", testEmptyThicknessUniformSeries},
        {"nonuniform slice spacing", testNonuniformSliceSpacing},
        {"empty thickness single slice rejection", testEmptyThicknessSingleSlice},
        {"empty series rejection", testEmptySeries},
        {"unsupported/inconsistent geometry", testRejectedGeometry},
    };
    for (const auto &test : tests) {
        if (!test.run(calibrationPath)) {
            std::cerr << "FAILED: " << test.name << "\n";
            return 1;
        }
    }
    std::cout << "Passed " << sizeof(tests)/sizeof(tests[0])
              << " CT geometry test groups\n";
    return 0;
}
