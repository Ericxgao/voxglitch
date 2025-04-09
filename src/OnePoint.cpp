#include "plugin.hpp"
#ifndef METAMODULE
#include "osdialog.h"
#else
#include "async_filebrowser.hh"
#endif
#include "settings.hpp"

#include "vgLib-2.0/constants.h"

#include "vgLib-2.0/components/VoxglitchComponents.hpp"
#include "vgLib-2.0/sequencer/Sequencer.hpp"
#include "vgLib-2.0/sequencer/GateSequencer.hpp"

using namespace vgLib_v2;

#include "OnePoint/OnePoint.hpp"
#include "OnePoint/OnePointReadoutWidget.hpp"
#include "OnePoint/OnePointWidget.hpp"

Model* modelOnePoint = createModel<OnePoint, OnePointWidget>("onepoint");
