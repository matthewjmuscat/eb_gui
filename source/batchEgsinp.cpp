#include "batchEgsinp.h"

#include "interface.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

namespace {
	struct BatchJobResult {
		int row = -1;
		QString jobId;
		QString vpm;
		QString transformation;
		QString vpmPath;
		QString transformationPath;
		QString isotope;
		QString source;
		QString airKerma;
		QString tg43Output;
		QString tg186Output;
		QString tg43Path;
		QString tg186Path;
		QStringList errors;
		QStringList warnings;
	};

	QString joinPath(const QString &dir, const QString &name) {
		if (dir.endsWith("/") || dir.endsWith("\\"))
			return dir + name;
		return dir + "/" + name;
	}

	QJsonArray stringListToJson(const QStringList &values) {
		QJsonArray array;
		for (int i = 0; i < values.size(); i++)
			array.append(values[i]);
		return array;
	}

	QString jobLabel(const BatchJobResult &job) {
		return job.jobId.isEmpty() ? QString("row_%1").arg(job.row) : job.jobId;
	}

	bool jobFails(const BatchJobResult &job, bool manifestBlocked) {
		return manifestBlocked || !job.errors.isEmpty();
	}

	QJsonObject jobToJson(const BatchJobResult &job, bool manifestBlocked) {
		QJsonObject object;
		object["row"] = job.row;
		object["job_id"] = job.jobId;
		object["vpm"] = job.vpm;
		object["transformation"] = job.transformation;
		object["vpm_path"] = job.vpmPath;
		object["transformation_path"] = job.transformationPath;
		object["isotope"] = job.isotope;
		object["source"] = job.source;
		object["air_kerma"] = job.airKerma;
		object["tg43_output"] = job.tg43Output;
		object["tg186_output"] = job.tg186Output;
		object["tg43_path"] = job.tg43Path;
		object["tg186_path"] = job.tg186Path;
		object["status"] = jobFails(job, manifestBlocked) ? "fail" : "pass";
		object["manifest_blocked"] = manifestBlocked;
		object["errors"] = stringListToJson(job.errors);
		object["warnings"] = stringListToJson(job.warnings);
		return object;
	}

	QString jsonString(const QJsonObject &object, const QString &key) {
		QJsonValue value = object.value(key);
		return value.isString() ? value.toString().trimmed() : QString();
	}

	bool selectExactListItem(QListWidget *list, const QString &name, QStringList *errors, const QString &label) {
		QList <QListWidgetItem*> matches = list->findItems(name, Qt::MatchExactly);
		if (matches.size() != 1) {
			if (matches.isEmpty())
				*errors << label + " not found: " + name;
			else
				*errors << label + " is ambiguous: " + name;
			list->clearSelection();
			list->setCurrentRow(-1);
			return false;
		}

		list->setCurrentItem(matches[0]);
		return true;
	}

	QString selectedPhantomPath(Interface *ui) {
		int row = ui->phantomListView->currentRow();
		if (row < 0)
			return QString();
		if (row < ui->data->localNamePhants.size())
			return joinPath(ui->data->localDirPhants[row], ui->data->localNamePhants[row]);

		int libIndex = row - ui->data->localNamePhants.size();
		if (libIndex < 0 || libIndex >= ui->data->libNamePhants.size())
			return QString();
		return joinPath(ui->data->libDirPhants[libIndex], ui->data->libNamePhants[libIndex]);
	}

	bool isSafeOutputName(const QString &name, QStringList *errors, const QString &label) {
		if (name.isEmpty()) {
			*errors << label + " output filename is empty.";
			return false;
		}
		if (!name.endsWith(".egsinp"))
			*errors << label + " output filename must end in .egsinp: " + name;
		if (QFileInfo(name).isAbsolute())
			*errors << label + " output filename must be relative: " + name;
		if (name.contains("/") || name.contains("\\"))
			*errors << label + " output filename must not contain path separators: " + name;
		if (name.contains(".."))
			*errors << label + " output filename must not contain '..': " + name;
		return errors->isEmpty();
	}

	void validateOutput(const QString &name, const QString &label, const QDir &outputDir, bool outputRootOk,
	                    QMap <QString, int> *seenOutputs, QVector <BatchJobResult> *jobs, int jobIndex) {
		BatchJobResult &job = (*jobs)[jobIndex];
		QStringList errors;
		isSafeOutputName(name, &errors, label);
		job.errors << errors;

		if (!name.isEmpty() && seenOutputs->contains(name)) {
			int firstIndex = seenOutputs->value(name);
			QString message = "Duplicate output filename: " + name;
			job.errors << message;
			if (firstIndex >= 0 && firstIndex < jobs->size())
				(*jobs)[firstIndex].errors << message;
		}
		else if (!name.isEmpty()) {
			seenOutputs->insert(name, jobIndex);
		}

		if (!name.isEmpty() && outputRootOk) {
			QString path = outputDir.filePath(name);
			if (label == "TG43")
				job.tg43Path = path;
			else
				job.tg186Path = path;
			if (QFileInfo::exists(path))
				job.errors << label + " output already exists: " + path;
		}
	}

	void printJobSummary(const BatchJobResult &job, bool manifestBlocked) {
		QTextStream out(stdout);
		if (!jobFails(job, manifestBlocked)) {
			out << jobLabel(job) << " PASS"
			    << " isotope=" << job.isotope
			    << " source=" << job.source
			    << " air_kerma=" << job.airKerma << "\n";
			return;
		}

		out << jobLabel(job) << " FAIL";
		for (int i = 0; i < job.errors.size(); i++)
			out << (i == 0 ? ": " : "; ") << job.errors[i];
		if (manifestBlocked)
			out << (job.errors.isEmpty() ? ": " : "; ") << "Blocked by manifest errors.";
		out << "\n";
	}

	bool writeJsonReport(const QString &path, const QJsonObject &report, QString *errorMessage) {
		QSaveFile file(path);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			*errorMessage = "Failed to open preflight report for writing: " + path + "\n" + file.errorString();
			return false;
		}

		QByteArray bytes = QJsonDocument(report).toJson(QJsonDocument::Indented);
		bytes.append('\n');
		if (file.write(bytes) != bytes.size()) {
			*errorMessage = "Failed to write complete preflight report: " + path + "\n" + file.errorString();
			return false;
		}
		if (!file.commit()) {
			*errorMessage = "Failed to commit preflight report: " + path + "\n" + file.errorString();
			return false;
		}
		return true;
	}
}

int runBatchEgsinpPreflight(Interface *ui, const QString &manifestPath, const QString &outputRoot, const QString &reportPath) {
	QTextStream out(stdout);
	QTextStream err(stderr);
	QFile manifestFile(manifestPath);
	if (!manifestFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		err << "Failed to open manifest: " << manifestPath << "\n" << manifestFile.errorString() << "\n";
		return 2;
	}

	QJsonParseError parseError;
	QJsonDocument manifestDocument = QJsonDocument::fromJson(manifestFile.readAll(), &parseError);
	if (parseError.error != QJsonParseError::NoError || !manifestDocument.isObject()) {
		err << "Failed to parse manifest JSON: " << parseError.errorString() << "\n";
		return 2;
	}

	QJsonObject manifest = manifestDocument.object();
	QJsonArray manifestJobs = manifest.value("jobs").toArray();
	QVector <BatchJobResult> jobs;
	QStringList manifestErrors;

	QJsonValue schemaVersion = manifest.value("schema_version");
	if (!manifest.contains("schema_version")) {
		manifestErrors << "Manifest schema_version is required and must be 1.";
	}
	else if (!schemaVersion.isDouble()) {
		manifestErrors << "Manifest schema_version must be the integer 1.";
	}
	else {
		int version = schemaVersion.toInt(-1);
		if (schemaVersion.toDouble() != static_cast<double>(version))
			manifestErrors << "Manifest schema_version must be the integer 1.";
		else if (version != 1)
			manifestErrors << QString("Unsupported manifest schema_version: %1").arg(version);
	}

	QDir outputDir(outputRoot);
	bool outputRootOk = outputDir.exists();
	if (outputRoot.trimmed().isEmpty())
		manifestErrors << "output-root is required.";
	else if (!outputRootOk)
		manifestErrors << "output-root does not exist or is not a directory: " + outputRoot;

	if (!manifest.value("jobs").isArray())
		manifestErrors << "Manifest must contain a jobs array.";
	else if (manifestJobs.isEmpty())
		manifestErrors << "Manifest jobs array must contain at least one job.";

	QMap <QString, int> seenOutputs;
	QMap <QString, int> seenJobIds;
	for (int i = 0; i < manifestJobs.size(); i++) {
		BatchJobResult job;
		job.row = i;

		if (!manifestJobs[i].isObject()) {
			job.jobId = QString("row_%1").arg(i);
			job.errors << "Job entry is not a JSON object.";
			jobs.append(job);
			continue;
		}

		QJsonObject object = manifestJobs[i].toObject();
		job.jobId = jsonString(object, "job_id");
		job.vpm = jsonString(object, "vpm");
		job.transformation = jsonString(object, "transformation");
		QJsonObject outputs = object.value("outputs").toObject();
		job.tg43Output = jsonString(outputs, "tg43");
		job.tg186Output = jsonString(outputs, "tg186");

		jobs.append(job);
		int jobIndex = jobs.size()-1;
		if (jobs[jobIndex].jobId.isEmpty()) {
			jobs[jobIndex].errors << "job_id is required.";
		}
		else if (seenJobIds.contains(jobs[jobIndex].jobId)) {
			int firstIndex = seenJobIds.value(jobs[jobIndex].jobId);
			QString message = "Duplicate job_id: " + jobs[jobIndex].jobId;
			jobs[jobIndex].errors << message;
			if (firstIndex >= 0 && firstIndex < jobs.size())
				jobs[firstIndex].errors << message;
		}
		else {
			seenJobIds.insert(jobs[jobIndex].jobId, jobIndex);
		}
		if (jobs[jobIndex].vpm.isEmpty())
			jobs[jobIndex].errors << "vpm is required.";
		if (jobs[jobIndex].transformation.isEmpty())
			jobs[jobIndex].errors << "transformation is required.";
		if (jobs[jobIndex].tg43Output == jobs[jobIndex].tg186Output && !jobs[jobIndex].tg43Output.isEmpty())
			jobs[jobIndex].errors << "TG43 and TG186 output filenames must be distinct.";

		validateOutput(jobs[jobIndex].tg43Output, "TG43", outputDir, outputRootOk, &seenOutputs, &jobs, jobIndex);
		validateOutput(jobs[jobIndex].tg186Output, "TG186", outputDir, outputRootOk, &seenOutputs, &jobs, jobIndex);

		if (!jobs[jobIndex].vpm.isEmpty() && selectExactListItem(ui->phantomListView, jobs[jobIndex].vpm, &jobs[jobIndex].errors, "VPM"))
			jobs[jobIndex].vpmPath = selectedPhantomPath(ui);

		if (!jobs[jobIndex].transformation.isEmpty() && selectExactListItem(ui->transformationListView, jobs[jobIndex].transformation, &jobs[jobIndex].errors, "Transformation")) {
			RunInputPreflightResult runInputs;
			ui->preflightSelectedRunInputs(&runInputs);
			jobs[jobIndex].transformationPath = runInputs.transformationPath;
			jobs[jobIndex].isotope = runInputs.isotope;
			jobs[jobIndex].source = runInputs.source;
			jobs[jobIndex].airKerma = runInputs.airKerma;
			jobs[jobIndex].errors << runInputs.errors;
		}
	}

	bool manifestBlocked = !manifestErrors.isEmpty();
	int passCount = 0;
	int failCount = 0;
	for (int i = 0; i < jobs.size(); i++) {
		if (!jobFails(jobs[i], manifestBlocked))
			passCount++;
		else
			failCount++;
	}

	QJsonArray jobArray;
	for (int i = 0; i < jobs.size(); i++)
		jobArray.append(jobToJson(jobs[i], manifestBlocked));

	QJsonObject summary;
	summary["total"] = jobs.size();
	summary["pass"] = passCount;
	summary["fail"] = failCount;

	QJsonObject report;
	report["schema_version"] = 1;
	report["status"] = manifestBlocked || failCount > 0 ? "fail" : "pass";
	report["mode"] = "preflight";
	report["manifest_path"] = QFileInfo(manifestPath).absoluteFilePath();
	report["output_root"] = QFileInfo(outputRoot).absoluteFilePath();
	report["manifest_errors"] = stringListToJson(manifestErrors);
	report["summary"] = summary;
	report["jobs"] = jobArray;

	QString finalReportPath = reportPath;
	if (finalReportPath.isEmpty())
		finalReportPath = outputDir.filePath("batch_egsinp_preflight_report.json");
	QString reportError;
	if (!writeJsonReport(finalReportPath, report, &reportError)) {
		err << reportError << "\n";
		return 2;
	}

	for (int i = 0; i < jobs.size(); i++)
		printJobSummary(jobs[i], manifestBlocked);

	out << "Preflight: " << passCount << " valid, " << failCount << " invalid";
	if (manifestBlocked)
		out << " (manifest invalid)";
	out << "\n";
	out << "No .egsinp files generated.\n";
	out << "report=" << finalReportPath << "\n";

	if (!manifestErrors.isEmpty()) {
		for (int i = 0; i < manifestErrors.size(); i++)
			err << "ERROR " << manifestErrors[i] << "\n";
	}

	return failCount == 0 && !manifestBlocked ? 0 : 1;
}