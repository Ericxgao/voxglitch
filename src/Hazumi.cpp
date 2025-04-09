#include "plugin.hpp"
#ifndef METAMODULE
#include "osdialog.h"
#else
#include "async_filebrowser.hh"
#endif
#include "settings.hpp"

#include "vgLib-2.0/constants.h"

#include "vgLib-2.0/components/VoxglitchComponents.hpp"

using namespace vgLib_v2;

#include "Hazumi/defines.h"
#include "Hazumi/HazumiSequencer.hpp"
#include "Hazumi/Hazumi.hpp"
#include "Hazumi/HazumiSequencerDisplay.hpp"
#include "Hazumi/HazumiWidget.hpp"

Model* modelHazumi = createModel<Hazumi, HazumiWidget>("hazumi");
