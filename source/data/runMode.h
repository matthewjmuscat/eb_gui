#ifndef RUN_MODE_H
#define RUN_MODE_H

#include <QString>

namespace RunMode {

QString resolve(
    const QString &selectedMode,
    bool tg43,
    bool permanentTreatment
);

}

#endif