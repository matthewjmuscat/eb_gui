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
    QCommandLineOption batchOption(QStringList() << "batch-egsinp", "Resolved JSON manifest for batch egsinp preflight.", "manifest");
    QCommandLineOption preflightOption(QStringList() << "preflight", "Run manifest-driven batch preflight without generating egsinp files.");
    QCommandLineOption outputRootOption(QStringList() << "output-root", "Directory where generated egsinp files would be written.", "directory");
    QCommandLineOption reportOption(QStringList() << "preflight-report", "Path for the JSON preflight report.", "report");
    parser.addOption(batchOption);
    parser.addOption(preflightOption);
    parser.addOption(outputRootOption);
    parser.addOption(reportOption);
    parser.process(app);

    if (parser.isSet(batchOption)) {
        if (!parser.isSet(preflightOption)) {
            std::cerr << "--batch-egsinp currently requires --preflight\n";
            return 2;
        }
        if (parser.value(outputRootOption).trimmed().isEmpty()) {
            std::cerr << "--output-root is required with --batch-egsinp --preflight\n";
            return 2;
        }

        Interface w(false);
        return runBatchEgsinpPreflight(&w, parser.value(batchOption), parser.value(outputRootOption), parser.value(reportOption));
    }

    Interface w;
    w.show();

    //connects everything
    return app.exec();
}