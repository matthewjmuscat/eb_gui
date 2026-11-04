#include "../source/data.h"

#include <QCoreApplication>

#include <cmath>
#include <cstring>
#include <iostream>

namespace {

void appendU16(QByteArray *bytes, quint16 value) {
    bytes->append(static_cast<char>(value & 0xff));
    bytes->append(static_cast<char>((value >> 8) & 0xff));
}

void appendU32(QByteArray *bytes, quint32 value) {
    bytes->append(static_cast<char>(value & 0xff));
    bytes->append(static_cast<char>((value >> 8) & 0xff));
    bytes->append(static_cast<char>((value >> 16) & 0xff));
    bytes->append(static_cast<char>((value >> 24) & 0xff));
}

QByteArray textElement(quint16 group, quint16 element, const char *vr, QString text) {
    QByteArray value = text.toLatin1();
    if (value.size() % 2)
        value.append(' ');

    QByteArray bytes;
    appendU16(&bytes, group);
    appendU16(&bytes, element);
    bytes.append(vr, 2);
    appendU16(&bytes, static_cast<quint16>(value.size()));
    bytes.append(value);
    return bytes;
}

QByteArray sequenceElement(
    quint16 group,
    quint16 element,
    const QVector<QByteArray> &items
) {
    QByteArray content;
    for (const QByteArray &item : items) {
        appendU16(&content, 0xfffe);
        appendU16(&content, 0xe000);
        appendU32(&content, static_cast<quint32>(item.size()));
        content.append(item);
    }

    QByteArray bytes;
    appendU16(&bytes, group);
    appendU16(&bytes, element);
    bytes.append("SQ\0\0", 4);
    appendU32(&bytes, static_cast<quint32>(content.size()));
    bytes.append(content);
    return bytes;
}

Attribute *valueAttribute(quint16 group, quint16 element, QString value) {
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

Attribute *sequenceAttribute(
    quint16 group,
    quint16 element,
    const QVector<QByteArray> &items
) {
    Attribute *attribute = new Attribute();
    attribute->tag[0] = group;
    attribute->tag[1] = element;
    attribute->vl = 0;
    for (const QByteArray &item : items) {
        unsigned char *data = new unsigned char[item.size()];
        if (!item.isEmpty())
            std::memcpy(data, item.constData(), item.size());
        attribute->seq.items.append(new SequenceItem(item.size(), data));
    }
    return attribute;
}

QByteArray controlPoint(QString positionMm, double cumulativeWeight) {
    return textElement(0x300a, 0x02d4, "DS", positionMm) +
           textElement(
               0x300a,
               0x02d6,
               "DS",
               QString::number(cumulativeWeight, 'g', 16)
           );
}

QByteArray temporaryChannel(
    double totalTimeSeconds,
    double finalWeight,
    const QVector<QByteArray> &controlPoints,
    bool includeFinalWeight = true,
    int referencedSourceNumber = 1
) {
    QByteArray item;
    if (referencedSourceNumber > 0) {
        item += textElement(
            0x300c,
            0x000e,
            "IS",
            QString::number(referencedSourceNumber)
        );
    }
    item += textElement(
        0x300a,
        0x0286,
        "DS",
        QString::number(totalTimeSeconds, 'g', 16)
    );
    if (includeFinalWeight) {
        item += textElement(
            0x300a,
            0x02c8,
            "DS",
            QString::number(finalWeight, 'g', 16)
        );
    }
    item += sequenceElement(0x300a, 0x02d0, controlPoints);
    return item;
}

QByteArray permanentChannel(
    const QVector<QByteArray> &controlPoints,
    int referencedSourceNumber = 1
) {
    return textElement(
               0x300c,
               0x000e,
               "IS",
               QString::number(referencedSourceNumber)
           )+
           sequenceElement(0x300a, 0x02d0, controlPoints);
}

void installPlan(
    Data *data,
    QString treatmentType,
    QString treatmentTechnique,
    const QVector<QByteArray> &channels,
    const QVector<int> &sourceNumbers = {1}
) {
    DICOM *plan = new DICOM(&data->tag_data);
    plan->isImplicit = false;

    QVector<QByteArray> sourceItems;
    for (int sourceNumber : sourceNumbers) {
        sourceItems.append(
            textElement(0x300a, 0x0212, "IS", QString::number(sourceNumber))+
            textElement(0x300a, 0x0226, "CS", "IR192")+
            textElement(0x300a, 0x0228, "DS", "73.83")+
            textElement(0x300a, 0x022a, "DS", "1.0")
        );
    }
    QByteArray applicationItem = sequenceElement(0x300a, 0x0280, channels);

    plan->data.append(valueAttribute(0x300a, 0x0200, treatmentTechnique));
    plan->data.append(valueAttribute(0x300a, 0x0202, treatmentType));
    plan->data.append(sequenceAttribute(0x300a, 0x0210, sourceItems));
    plan->data.append(sequenceAttribute(0x300a, 0x0230, {applicationItem}));
    data->plan_data = plan;
}

bool closeTo(double left, double right, double tolerance = 1.0e-6) {
    return std::abs(left-right) <= tolerance;
}

bool testMultiChannelHdrImport() {
    const double firstDwell = 32.7556;
    const double secondDwell = 34.3406;
    const double thirdDwell = 32.2273;
    const double firstChannelTime = firstDwell+secondDwell;
    const double firstFinalWeight = 2.0;
    const double firstWeight = firstDwell*firstFinalWeight/firstChannelTime;

    QByteArray firstChannel = temporaryChannel(
        firstChannelTime,
        firstFinalWeight,
        {
            controlPoint("10\\20\\30", 0.0),
            controlPoint("10\\20\\30", firstWeight),
            controlPoint("40\\50\\60", firstWeight),
            controlPoint("40\\50\\60", firstFinalWeight),
        }
    );
    QByteArray secondChannel = temporaryChannel(
        thirdDwell,
        7.0,
        {
            controlPoint("-10\\-20\\-30", 0.0),
            controlPoint("-10\\-20\\-30", 7.0),
        }
    );

    Data data;
    installPlan(&data, "HDR", "INTERSTITIAL", {firstChannel, secondChannel});
    QString log;
    int result = data.parsePlan(&log);
    const double expectedTotal = firstDwell+secondDwell+thirdDwell;
    return result == 0 && data.seedPos.size() == 3 && data.seedTime.size() == 3 &&
           data.seedPos[0] == QVector3D(1, 2, 3) &&
           data.seedPos[1] == QVector3D(4, 5, 6) &&
           data.seedPos[2] == QVector3D(-1, -2, -3) &&
           closeTo(data.seedTime[0], firstDwell) &&
           closeTo(data.seedTime[1], secondDwell) &&
           closeTo(data.seedTime[2], thirdDwell) &&
           closeTo(data.treatmentTime*3600.0, expectedTotal);
}

bool testMissingFinalWeightRollback() {
    QByteArray channel = temporaryChannel(
        10.0,
        10.0,
        {
            controlPoint("0\\0\\0", 0.0),
            controlPoint("0\\0\\0", 10.0),
        },
        false
    );

    Data data;
    installPlan(&data, "HDR", "INTERSTITIAL", {channel});
    QString log;
    int result = data.parsePlan(&log);
    return result == 306 && data.seedPos.isEmpty() && data.seedTime.isEmpty() &&
           closeTo(data.treatmentTime, 0.0);
}

bool testPermanentLdrImport() {
    QByteArray channel = permanentChannel(
        {
            controlPoint("0\\0\\0", 1.0),
            controlPoint("10\\0\\0", 3.0),
        }
    );

    Data data;
    installPlan(&data, "LDR", "PERMANENT", {channel});
    QString log;
    int result = data.parsePlan(&log);
    return result == 0 && data.seedPos.size() == 1 && data.seedTime.size() == 1 &&
           closeTo(data.seedPos[0].x(), 0.75) &&
           closeTo(data.seedPos[0].y(), 0.0) &&
           closeTo(data.seedPos[0].z(), 0.0) &&
           closeTo(data.seedTime[0], 4.0) && closeTo(data.treatmentTime, 0.0);
}

bool testInvalidSourceReferences() {
    QVector<QByteArray> controlPoints = {
        controlPoint("0\\0\\0", 0.0),
        controlPoint("0\\0\\0", 1.0),
    };

    Data missingReference;
    installPlan(
        &missingReference,
        "HDR",
        "INTERSTITIAL",
        {temporaryChannel(1.0, 1.0, controlPoints, true, 0)}
    );
    QString log;
    if (missingReference.parsePlan(&log) != 312 || !missingReference.seedPos.isEmpty())
        return false;

    Data unknownReference;
    installPlan(
        &unknownReference,
        "HDR",
        "INTERSTITIAL",
        {temporaryChannel(1.0, 1.0, controlPoints, true, 2)}
    );
    if (unknownReference.parsePlan(&log) != 312 || !unknownReference.seedPos.isEmpty())
        return false;

    Data multipleSources;
    installPlan(
        &multipleSources,
        "HDR",
        "INTERSTITIAL",
        {temporaryChannel(1.0, 1.0, controlPoints)},
        {1, 2}
    );
    return multipleSources.parsePlan(&log) == 311 && multipleSources.seedPos.isEmpty();
}

}

int main(int argc, char *argv[]) {
    QCoreApplication application(argc, argv);
    const struct {
        const char *name;
        bool (*run)();
    } tests[] = {
        {"multi-channel HDR RTPLAN import", testMultiChannelHdrImport},
        {"missing final weight rollback", testMissingFinalWeightRollback},
        {"permanent LDR RTPLAN import", testPermanentLdrImport},
        {"invalid source reference rejection", testInvalidSourceReferences},
    };

    for (const auto &test : tests) {
        if (!test.run()) {
            std::cerr << "FAILED: " << test.name << "\n";
            return 1;
        }
    }
    std::cout << "Passed " << sizeof(tests)/sizeof(tests[0])
              << " RTPLAN import tests\n";
    return 0;
}
