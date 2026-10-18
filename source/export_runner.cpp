/*
################################################################################
#
#  egs_brachy_GUI export_runner.cpp
#
################################################################################
*/

#include "export_runner.h"

#include "data.h"

#include <QtCore>
#include <iostream>

struct ExportContour {
	QString contourName;
	QString maskPath;
	QString metricPreset;
};

struct ExportJobResult {
	QString patientId;
	QString mode;
	QString status;
	QStringList errors;
	QStringList warnings;
	QStringList outputs;
};

static QString jsonString(const QJsonObject &object, QString key) {
	return object.value(key).toString().trimmed();
}

static QJsonObject jsonObject(const QJsonObject &object, QString key) {
	return object.value(key).toObject();
}

static QString nestedPath(const QJsonObject &object, QString key) {
	return jsonObject(object, key).value("path").toString().trimmed();
}

static bool ensureDirectory(QString path, QStringList *errors) {
	QDir dir(path);
	if (dir.exists())
		return true;
	if (dir.mkpath("."))
		return true;
	errors->append("Could not create output directory: " + path);
	return false;
}

static bool writeTextFile(QString path, QString text, QStringList *errors) {
	QFileInfo info(path);
	QDir().mkpath(info.absolutePath());
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		errors->append("Could not write output file: " + path);
		return false;
	}
	QTextStream out(&file);
	out << text;
	return true;
}

static QString egsphantStem(QString path) {
	QString stem = QFileInfo(path).absolutePath() + "/" + QFileInfo(path).fileName();
	if (stem.endsWith(".gz"))
		stem = stem.left(stem.size()-3);
	if (stem.endsWith(".egsphant"))
		stem = stem.left(stem.size()-9);
	else if (stem.endsWith(".begsphant"))
		stem = stem.left(stem.size()-10);
	else if (stem.endsWith(".geom"))
		stem = stem.left(stem.size()-5);
	return stem;
}

static bool copyIfPresent(QString source, QString destination, QStringList *warnings, QStringList *outputs, bool warnIfMissing = true) {
	if (source.isEmpty())
		return false;
	QFileInfo sourceInfo(source);
	if (!sourceInfo.isFile()) {
		if (warnIfMissing)
			warnings->append("Copy source missing: " + source);
		return false;
	}
	QDir().mkpath(QFileInfo(destination).absolutePath());
	if (QFileInfo(destination).exists()) {
		warnings->append("Copy destination already exists, leaving unchanged: " + destination);
		return false;
	}
	if (!QFile(source).copy(destination)) {
		warnings->append("Could not copy " + source + " to " + destination);
		return false;
	}
	outputs->append(destination);
	return true;
}

static QString readDoseScaling(QString egsinpPath) {
	QFile egsinp(egsinpPath);
	if (!egsinp.open(QIODevice::ReadOnly | QIODevice::Text))
		return "";
	QTextStream input(&egsinp);
	while (!input.atEnd()) {
		QString line = input.readLine();
		if (line.contains("dose scaling factor") && line.contains("="))
			return line.split("=")[1].split("#")[0].trimmed();
	}
	return "";
}

static QVector<ExportContour> contoursFromJob(const QJsonObject &job, QStringList *errors) {
	QVector<ExportContour> contours;
	QJsonArray rawContours = jsonObject(job.value("export_options").toObject(), "contours").value("items").toArray();
	if (rawContours.isEmpty())
		rawContours = jsonObject(job, "export_options").value("contours").toArray();
	for (int i = 0; i < rawContours.size(); i++) {
		QJsonObject rawContour = rawContours[i].toObject();
		ExportContour contour;
		contour.contourName = jsonString(rawContour, "contour_name");
		contour.maskPath = jsonString(rawContour, "mask_path");
		contour.metricPreset = jsonString(rawContour, "metric_preset");
		if (contour.contourName.isEmpty() || contour.maskPath.isEmpty() || contour.metricPreset.isEmpty())
			errors->append("Contour entry is missing contour_name, mask_path, or metric_preset");
		else
			contours.append(contour);
	}
	if (contours.isEmpty())
		errors->append("Export job has no contours");
	return contours;
}

static QJsonObject jobResultJson(const ExportJobResult &result) {
	QJsonObject output;
	output["patient_id"] = result.patientId;
	output["mode"] = result.mode;
	output["status"] = result.status;
	output["errors"] = QJsonArray::fromStringList(result.errors);
	output["warnings"] = QJsonArray::fromStringList(result.warnings);
	output["outputs"] = QJsonArray::fromStringList(result.outputs);
	return output;
}

static void copySupportFiles(const QJsonObject &job, ExportJobResult *result) {
	QString outputDir = jsonString(job, "output_dir");
	QJsonObject inputs = jsonObject(job, "inputs");
	QString egsphantPath = jsonString(inputs, "egsphant_path");
	QString transformationPath = jsonString(inputs, "transformation_path");
	QJsonObject rawOutputs = jsonObject(inputs, "raw_outputs");
	bool copyRawSimulation = jsonObject(job, "export_options").value("copy_raw_simulation_to_patient").toBool(false);

	copyIfPresent(egsphantPath, outputDir + "/phantom/" + QFileInfo(egsphantPath).fileName(), &result->warnings, &result->outputs);
	QString phantStem = egsphantStem(egsphantPath);
	copyIfPresent(phantStem + ".log", outputDir + "/phantom/" + QFileInfo(phantStem).fileName() + ".log", &result->warnings, &result->outputs);

	copyIfPresent(transformationPath, outputDir + "/plan/" + QFileInfo(transformationPath).fileName(), &result->warnings, &result->outputs);
	copyIfPresent(transformationPath + ".log", outputDir + "/plan/" + QFileInfo(transformationPath).fileName() + ".log", &result->warnings, &result->outputs);
	copyIfPresent(transformationPath + ".dwell", outputDir + "/plan/" + QFileInfo(transformationPath).fileName() + ".dwell", &result->warnings, &result->outputs, false);
	copyIfPresent(transformationPath + ".activity", outputDir + "/plan/" + QFileInfo(transformationPath).fileName() + ".activity", &result->warnings, &result->outputs, false);

	if (!copyRawSimulation)
		return;
	for (QString extension : {".egsinp", ".egslog", ".phantom.3ddose", ".egsdat"}) {
		QString source = nestedPath(rawOutputs, extension);
		copyIfPresent(source, outputDir + "/simulation/" + QFileInfo(source).fileName(), &result->warnings, &result->outputs);
	}
}

static ExportJobResult runOneExportJob(const QJsonObject &job, Data *data) {
	ExportJobResult result;
	result.patientId = jsonString(job, "patient_id");
	result.mode = jsonString(job, "mode");
	result.status = "failed";

	if (jsonString(job, "status") != "ready") {
		result.status = "skipped";
		result.errors.append("Manifest job is not ready");
		return result;
	}

	QString outputDir = jsonString(job, "output_dir");
	if (!ensureDirectory(outputDir, &result.errors))
		return result;

	QJsonObject inputs = jsonObject(job, "inputs");
	QJsonObject exportOptions = jsonObject(job, "export_options");
	QString dosePath = nestedPath(inputs, "dose_file_path");
	QString egsinpPath = nestedPath(jsonObject(inputs, "raw_outputs"), ".egsinp");
	QVector<ExportContour> contours = contoursFromJob(job, &result.errors);
	if (!result.errors.isEmpty())
		return result;
	QFileInfo doseInfo(dosePath);
	if (!doseInfo.isFile() || !doseInfo.isReadable()) {
		result.errors.append("Selected dose file is missing or unreadable: " + dosePath);
		return result;
	}

	Dose doseData;
	if (dosePath.endsWith(".b3ddose"))
		doseData.readBIn(dosePath, 4);
	else if (dosePath.endsWith(".3ddose"))
		doseData.readIn(dosePath, 4);
	else {
		result.errors.append("Selected dose file is not of type 3ddose or b3ddose: " + dosePath);
		return result;
	}

	copySupportFiles(job, &result);

	QVector<QVector<DV> > doseValues;
	QVector<double> volumes;
	QVector<EGSPhant*> masks;
	QVector<QString> dxValues;
	QVector<QString> dccValues;
	QVector<QString> vxValues;
	QVector<QString> prescriptionValues;
	doseValues.resize(contours.size());
	volumes.resize(contours.size());
	masks.resize(contours.size());
	dxValues.resize(contours.size());
	dccValues.resize(contours.size());
	vxValues.resize(contours.size());
	prescriptionValues.resize(contours.size());

	for (int i = 0; i < contours.size(); i++) {
		int metricIndex = data->metricNames.indexOf(contours[i].metricPreset);
		if (metricIndex < 0) {
			result.errors.append("Metric preset not found: " + contours[i].metricPreset);
			continue;
		}
		masks[i] = new EGSPhant();
		masks[i]->loadgzEGSPhantFile(contours[i].maskPath);
		dxValues[i] = data->metricDx[metricIndex];
		dccValues[i] = data->metricDcc[metricIndex];
		vxValues[i] = data->metricVx[metricIndex];
		prescriptionValues[i] = data->metricDp[metricIndex];
	}
	if (!result.errors.isEmpty()) {
		qDeleteAll(masks);
		return result;
	}

	doseData.getDVs(&doseValues, &masks, &volumes);

	for (int i = 0; i < contours.size(); i++) {
		QString csvText = doseData.getMetricCSV(&(doseValues[i]), volumes[i], contours[i].contourName,
										 dxValues[i], dccValues[i], vxValues[i], prescriptionValues[i]);
		QString path = outputDir + "/" + contours[i].contourName + "_metrics.csv";
		if (writeTextFile(path, csvText, &result.errors))
			result.outputs.append(path);
	}

	if (exportOptions.value("output_dvh").toBool(true)) {
		for (int i = 0; i < contours.size(); i++) {
			if (doseValues[i].isEmpty()) {
				result.errors.append("No dose values available for DVH: " + contours[i].contourName);
				continue;
			}
			QString csvText = contours[i].contourName + ",\n";
			csvText += "dose / Gy, volume / %\n";
			int stride = 1;
			if (doseValues[i].size() > 200)
				stride = doseValues[i].size()/200.0;
			for (int j = 0; j < doseValues[i].size(); j++) {
				if (!(j%stride))
					csvText += QString::number(doseValues[i][j].dose)+","+
							   QString::number(100.0*double(doseValues[i].size()-j)/double(doseValues[i].size()))+"\n";
			}
			if ((doseValues[i].size()%stride))
				csvText += QString::number(doseValues[i].last().dose)+","+
						   QString::number(100.0/double(doseValues[i].size()))+"\n";
			QString path = outputDir + "/" + contours[i].contourName + "_DVH.csv";
			if (writeTextFile(path, csvText, &result.errors))
				result.outputs.append(path);
		}
	}

	if (exportOptions.value("output_differential").toBool(true)) {
		int binCount = data->histogramBinCount;
		if (binCount <= 0)
			result.errors.append("histogram bin count must be positive");
		for (int i = 0; i < contours.size() && binCount > 0; i++) {
			if (doseValues[i].isEmpty()) {
				result.errors.append("No dose values available for differential histogram: " + contours[i].contourName);
				continue;
			}
			QString csvText = contours[i].contourName + ",\n";
			csvText += "dose / Gy, Volume / voxels\n";
			double stride = (doseValues[i].last().dose-doseValues[i][0].dose)/double(binCount);
			double startDose = doseValues[i][0].dose;
			double previous = startDose;
			int dataIndex = 0;
			for (int j = 1; j <= binCount; j++) {
				double current = startDose+stride*j;
				int dataCount = 0;
				while (dataIndex < doseValues[i].size()) {
					if (doseValues[i][dataIndex].dose > current)
						break;
					dataCount++;
					dataIndex++;
				}
				csvText += QString::number((current+previous)/2.0)+","+QString::number(dataCount)+"\n";
				previous = current;
			}
			QString path = outputDir + "/" + contours[i].contourName + "_diff.csv";
			if (writeTextFile(path, csvText, &result.errors))
				result.outputs.append(path);
		}
	}

	if (exportOptions.value("output_rt_dose").toBool(false)) {
		QString rtDosePath = outputDir + "/output_rt_dose.dose.dcm";
		QString rtErrorPath = outputDir + "/output_rt_dose.error.dcm";
		data->outputRTDose(rtDosePath, rtErrorPath, &doseData, readDoseScaling(egsinpPath), 3.0);
		result.outputs.append(rtDosePath);
		result.outputs.append(rtErrorPath);
	}

	qDeleteAll(masks);
	result.status = result.errors.isEmpty() ? "exported" : "failed";
	return result;
}

int runExportManifest(QString manifestPath, QString reportPath) {
	QFile manifestFile(manifestPath);
	if (!manifestFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		std::cerr << "Could not read export manifest: " << manifestPath.toStdString() << std::endl;
		return 2;
	}
	QJsonParseError parseError;
	QJsonDocument manifestDoc = QJsonDocument::fromJson(manifestFile.readAll(), &parseError);
	if (parseError.error != QJsonParseError::NoError || !manifestDoc.isObject()) {
		std::cerr << "Invalid export manifest JSON: " << parseError.errorString().toStdString() << std::endl;
		return 2;
	}

	Data data;
	int defaultsError = data.loadDefaults();
	if (defaultsError) {
		std::cerr << "Could not load eb_gui defaults; error " << defaultsError << std::endl;
		return 2;
	}

	QJsonObject manifest = manifestDoc.object();
	QJsonArray jobs = manifest.value("export_jobs").toArray();
	QJsonArray reportJobs;
	int exportedCount = 0;
	int failedCount = 0;
	int skippedCount = 0;
	for (int i = 0; i < jobs.size(); i++) {
		ExportJobResult result = runOneExportJob(jobs[i].toObject(), &data);
		if (result.status == "exported")
			exportedCount++;
		else if (result.status == "skipped")
			skippedCount++;
		else
			failedCount++;
		reportJobs.append(jobResultJson(result));
		std::cout << result.status.toStdString() << " PT" << result.patientId.toStdString()
				  << " " << result.mode.toStdString() << std::endl;
	}

	QJsonObject summary;
	summary["exported_count"] = exportedCount;
	summary["failed_count"] = failedCount;
	summary["skipped_count"] = skippedCount;
	summary["job_count"] = jobs.size();

	QJsonObject report;
	report["schema_version"] = 1;
	report["kind"] = "cihr_ebgui_export_report";
	report["manifest_path"] = manifestPath;
	report["summary"] = summary;
	report["jobs"] = reportJobs;

	if (!reportPath.isEmpty()) {
		QFileInfo reportInfo(reportPath);
		QDir().mkpath(reportInfo.absolutePath());
		QFile reportFile(reportPath);
		if (!reportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			std::cerr << "Could not write export report: " << reportPath.toStdString() << std::endl;
			return 2;
		}
		reportFile.write(QJsonDocument(report).toJson(QJsonDocument::Indented));
	}

	std::cout << "exported=" << exportedCount << " failed=" << failedCount << " skipped=" << skippedCount << std::endl;
	return failedCount ? 1 : 0;
}