#include "plugin.h"
#include "presagepredictor.h"

#include <qqml.h>

void PresagePredictorPlugin::registerTypes(const char *uri)
{
    // @uri hu.mm.PresagePredictor
    qRegisterMetaType<PresagePredictorModel*>("PresagePredictorModel");
    qmlRegisterType<PresagePredictor>(uri, 1, 0, "PresagePredictor");
}

