#include "../source/data/DICOM.h"

#include <QBuffer>
#include <QCoreApplication>
#include <QDataStream>

#include <iostream>

namespace {

void appendUInt16(QByteArray &data, quint16 value) {
    data.append(static_cast<char>(value & 0xFF));
    data.append(static_cast<char>((value >> 8) & 0xFF));
}

void appendUInt32(QByteArray &data, quint32 value) {
    data.append(static_cast<char>(value & 0xFF));
    data.append(static_cast<char>((value >> 8) & 0xFF));
    data.append(static_cast<char>((value >> 16) & 0xFF));
    data.append(static_cast<char>((value >> 24) & 0xFF));
}

void appendTag(QByteArray &data, quint16 group, quint16 element) {
    appendUInt16(data, group);
    appendUInt16(data, element);
}

void appendDelimiter(QByteArray &data, quint16 element) {
    appendTag(data, 0xFFFE, element);
    appendUInt32(data, 0);
}

void appendUndefinedItem(QByteArray &data) {
    appendTag(data, 0xFFFE, 0xE000);
    appendUInt32(data, 0xFFFFFFFF);
    data.append("DATA", 4);
    appendDelimiter(data, 0xE00D);
}

bool testUndefinedSequence(DICOM &dicom) {
    QByteArray data;
    appendUndefinedItem(data);
    appendDelimiter(data, 0xE0DD);

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    Attribute attribute;

    int result = dicom.readSequence(&stream, &attribute);
    if (result != 1 || buffer.pos() != data.size() || attribute.seq.items.size() != 1) {
        std::cerr << "Undefined sequence suffix traversal failed: result=" << result
                  << ", position=" << buffer.pos() << "/" << data.size()
                  << ", items=" << attribute.seq.items.size() << "\n";
        return false;
    }
    return true;
}

bool testDefinedSequenceReader(DICOM &dicom) {
    QByteArray data;
    appendUndefinedItem(data);

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    Attribute attribute;

    int result = dicom.readDefinedSequence(&stream, &attribute, data.size());
    if (result != 1 || buffer.pos() != data.size() || attribute.seq.items.size() != 1) {
        std::cerr << "Defined sequence suffix traversal failed: result=" << result
                  << ", position=" << buffer.pos() << "/" << data.size()
                  << ", items=" << attribute.seq.items.size() << "\n";
        return false;
    }
    return true;
}

} // namespace

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    database tagDatabase;
    DICOM dicom(&tagDatabase);

    bool success = testUndefinedSequence(dicom);
    success = testDefinedSequenceReader(dicom) && success;
    return success ? 0 : 1;
}