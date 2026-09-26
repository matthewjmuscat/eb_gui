/*
################################################################################
#
#  egs_brachy_GUI main.cpp
#  Copyright (C) 2021 Shannon Jarvis, Martin Martinov, and Rowan Thomson
#
#  This file is part of egs_brachy_GUI
#
#  egs_brachy_GUI is free software: you can redistribute it and/or modify it
#  under the terms of the GNU Affero General Public License as published
#  by the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  egs_brachy_GUI is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Affero General Public License for more details:
#  <http://www.gnu.org/licenses/>.
#
################################################################################
#
#  When egs_brachy is used for publications, please cite our paper:
#  M. J. P. Chamberland, R. E. P. Taylor, D. W. O. Rogers, and R. M. Thomson,
#  egs brachy: a versatile and fast Monte Carlo code for brachytherapy,
#  Phys. Med. Biol. 61, 8214-8231 (2016).
#
#  When egs_brachy_GUI is used for publications, please cite our paper:
#  To Be Announced
#
################################################################################
#
#  Author:        Shannon Jarvis
#                 Martin Martinov (martinov@physics.carleton.ca)
#
#  Contributors:  Rowan Thomson (rthomson@physics.carleton.ca)
#
################################################################################
*/
#include "interface.h"
#include "batchEgsinp.h"
#include "GUI/ebInterface.h"
#include "GUI/phantInterface.h"
#include "GUI/sourceInterface.h"

#include <QtGui>
#include <QCommandLineParser>
#include <iostream>
#include <math.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("eb_gui");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("egs_brachy GUI");
    parser.addHelpOption();
    QCommandLineOption batchOption(QStringList() << "batch-egsinp", "Resolved JSON manifest for batch egsinp preflight or generation.", "manifest");
    QCommandLineOption preflightOption(QStringList() << "preflight", "Run manifest-driven batch preflight without generating egsinp files.");
    QCommandLineOption generateOption(QStringList() << "generate", "Generate egsinp files from a resolved manifest after full-batch preflight.");
    QCommandLineOption outputRootOption(QStringList() << "output-root", "Directory where generated egsinp files would be written.", "directory");
    QCommandLineOption reportOption(QStringList() << "batch-report", "Path for the JSON batch report.", "report");
    QCommandLineOption preflightReportOption(QStringList() << "preflight-report", "Path for the JSON preflight report.", "report");
    parser.addOption(batchOption);
    parser.addOption(preflightOption);
    parser.addOption(generateOption);
    parser.addOption(outputRootOption);
    parser.addOption(reportOption);
    parser.addOption(preflightReportOption);
    parser.process(app);

    if (parser.isSet(batchOption)) {
        if (parser.isSet(preflightOption) == parser.isSet(generateOption)) {
            std::cerr << "--batch-egsinp requires exactly one of --preflight or --generate\n";
            return 2;
        }
        if (parser.value(outputRootOption).trimmed().isEmpty()) {
            std::cerr << "--output-root is required with --batch-egsinp\n";
            return 2;
        }
        if (parser.isSet(reportOption) && parser.isSet(preflightReportOption)) {
            std::cerr << "Use only one of --batch-report or --preflight-report\n";
            return 2;
        }

        QString reportPath = parser.isSet(reportOption) ? parser.value(reportOption) : parser.value(preflightReportOption);
        Interface w(false);
        if (parser.isSet(generateOption))
            return runBatchEgsinpGenerate(&w, parser.value(batchOption), parser.value(outputRootOption), reportPath);
        return runBatchEgsinpPreflight(&w, parser.value(batchOption), parser.value(outputRootOption), reportPath);
    }

    Interface w;
    w.show();

    //connects everything
    return app.exec();
}