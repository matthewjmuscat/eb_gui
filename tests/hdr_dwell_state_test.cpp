#include "../source/interface.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include <cmath>
#include <iostream>

namespace {

bool writeText(const QString &path, const QString &text) {
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;
	QTextStream output(&file);
	output << text;
	return true;
}

bool closeTo(double left, double right, double tolerance = 1.0e-8) {
	return std::abs(left-right) <= tolerance;
}

}

int main(int argc, char *argv[]) {
	QApplication application(argc, argv);
	Interface interface;
	interface.hide();

	QString root = QCoreApplication::applicationDirPath()+"/synthetic_hdr_dwell";
	QDir().mkpath(root);
	QString transformationPath = root+"/stepping_transform";
	QString dwellPath = transformationPath+".dwell";

	if (!writeText(
			transformationPath,
			":start transformation:\n:stop transformation:\n"
			":start transformation:\n:stop transformation:\n"
			":start transformation:\n:stop transformation:\n"
		) ||
		!writeText(dwellPath, "1\n3\n2\n")) {
		std::cerr << "Failed to create synthetic dwell-state fixtures\n";
		return 1;
	}

	interface.data->localNameTransforms.clear();
	interface.data->localDirTransforms.clear();
	interface.data->localNameTransforms << "stepping_transform";
	interface.data->localDirTransforms << root+"/";
	interface.transformationRepopulate();
	interface.transformationListView->setCurrentRow(0);

	interface.transformationDwellEdit->setText("stale.dwell");
	interface.maxDwellTime = 99.0;
	interface.transformationLoadDwellsAuto(0);

	if (interface.transformationDwellEdit->text() != dwellPath ||
		!closeTo(interface.maxDwellTime, 3.0)) {
		std::cerr << "Reproduced stale automatic dwell-loading state: path="
				  << interface.transformationDwellEdit->text().toStdString()
				  << " max=" << interface.maxDwellTime << "\n";
		return 1;
	}

	interface.transformationDwell->blockSignals(true);
	interface.transformationDwell->setChecked(true);
	interface.transformationDwell->blockSignals(false);
	QString error;
	if (!interface.applyDwellStateToInput(&error)) {
		std::cerr << "Failed to apply valid dwell state: " << error.toStdString() << "\n";
		return 1;
	}
	interface.egsinp->SO_scale = QString::number(
		4.0/2.0*interface.dwellTimeScaleHours(), 'g', 16
	);
	QString input = interface.egsinp->buildInput();
	double expectedScale = 4.0/2.0*3.0/3600.0;
	if (!input.contains("source weights = 1 3 2") ||
		!closeTo(interface.egsinp->SO_scale.toDouble(), expectedScale)) {
		std::cerr << "Generated input has incorrect dwell weights or scaling\n";
		return 1;
	}

	QFile::remove(dwellPath);
	QString activityPath = transformationPath+".activity";
	if (!writeText(activityPath, "4 2 1\n"))
		return 1;
	interface.transformationLoadDwellsAuto(0);
	if (interface.dwellFilePath != QFileInfo(activityPath).absoluteFilePath() ||
		interface.dwellFileType != "activity" ||
		interface.dwellValues != "4 2 1" || !closeTo(interface.maxDwellTime, 4.0)) {
		std::cerr << "Activity fallback did not establish consistent state\n";
		return 1;
	}

	QString invalidPath = root+"/invalid.dwell";
	const QStringList invalidContents = {"", "1 nope 2", "0 0 0", "1 -2 3", "1 nan 2", "1 2"};
	for (const QString &content : invalidContents) {
		if (!writeText(invalidPath, content))
			return 1;
		error.clear();
		if (interface.loadDwellState(invalidPath, transformationPath, &error) ||
			!interface.dwellFilePath.isEmpty() || !interface.dwellValues.isEmpty() ||
			!closeTo(interface.maxDwellTime, 0.0)) {
			std::cerr << "Invalid dwell input left committed state\n";
			return 1;
		}
	}
	if (!writeText(invalidPath, "1 0 2") ||
		!interface.loadDwellState(invalidPath, transformationPath, &error) ||
		interface.dwellValues != "1 0 2" || !closeTo(interface.maxDwellTime, 2.0)) {
		std::cerr << "Valid zero activity weight was not preserved\n";
		return 1;
	}

	error.clear();
	if (interface.loadDwellState(root+"/missing.dwell", transformationPath, &error)) {
		std::cerr << "Missing dwell input was accepted\n";
		return 1;
	}

	if (!interface.loadDwellState(activityPath, transformationPath, &error) ||
		!writeText(activityPath, "broken\n"))
		return 1;
	if (interface.applyDwellStateToInput(&error) ||
		!interface.egsinp->sourceDwells.isEmpty() ||
		!closeTo(interface.maxDwellTime, 0.0)) {
		std::cerr << "Changed dwell file did not fail atomically\n";
		return 1;
	}

	interface.transformationDwell->blockSignals(true);
	interface.transformationDwell->setChecked(false);
	interface.transformationDwell->blockSignals(false);
	interface.clearDwellState();
	interface.sourcePermTime->setChecked(true);
	if (!interface.applyDwellStateToInput(&error) ||
		!interface.egsinp->sourceDwells.isEmpty() ||
		!closeTo(interface.maxDwellTime, 0.0)) {
		std::cerr << "Disabled dwell mode retained temporary-treatment state\n";
		return 1;
	}
	interface.sourcePermTime->setChecked(false);
	if (interface.applyDwellStateToInput(&error)) {
		std::cerr << "Temporary treatment accepted missing dwell state\n";
		return 1;
	}

	delete interface.egsinp;
	interface.egsinp = nullptr;
	delete interface.data;
	interface.data = nullptr;
	std::cout << "Passed HDR dwell application-state test\n";
	return 0;
}
