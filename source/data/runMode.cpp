#include "runMode.h"

namespace RunMode {

QString resolve(
    const QString &selectedMode,
    bool tg43,
    bool permanentTreatment
) {
    return tg43 || !permanentTreatment ? "superposition" : selectedMode;
}

}