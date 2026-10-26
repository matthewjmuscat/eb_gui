#include "../source/data/DICOM.h"

#include <QCoreApplication>
#include <QFile>
#include <QTemporaryDir>

#include <iostream>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    QByteArray fixture(128, '\0');
    fixture.append("DICM", 4);
    fixture.append("\xFE\xFF\xDD\xE0", 4);

    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        std::cerr << "Failed to create temporary test directory\n";
        return 1;
    }

    QFile fixtureFile(tempDir.filePath("unexpected-sequence-delimiter.dcm"));
    if (!fixtureFile.open(QIODevice::WriteOnly) || fixtureFile.write(fixture) != fixture.size()) {
        std::cerr << "Failed to write synthetic DICOM fixture\n";
        return 1;
    }
    fixtureFile.close();

    database tagDatabase;
    DICOM dicom(&tagDatabase);
    int result = dicom.parse(fixtureFile.fileName());
    if (result != 202) {
        std::cerr << "Expected parser error 202, got " << result << "\n";
        return 1;
    }

    return 0;
}