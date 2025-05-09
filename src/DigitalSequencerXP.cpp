//
// Voxglitch "DigitalSequencer" module for VCV Rack
// by Bret Truchan
//
// Probably influenced by Nord Modular and Reaktor, but it's been too long for
// me to remember.
//
// Special thanks to Artem Leonov for his testing and suggestions.
// Special thanks to Marc Boulé for his help with reset trigger behavior.
// Special thanks to the entire VCV Rack community for their support.
//

#include "plugin.hpp"
#ifndef METAMODULE
#include "osdialog.h"
#else
#include "async_filebrowser.hh"
#endif
#include "settings.hpp"

// These two includes are required for Mac
#include <fstream>
#include <array>

#include "vgLib-2.0/constants.h"

#include "vgLib-2.0/components/VoxglitchComponents.hpp"
#include "vgLib-2.0/sequencer/VoltageSequencerHistory.hpp"
#include "vgLib-2.0/sequencer/Sequencer.hpp"
#include "vgLib-2.0/sequencer/VoltageSequencer.hpp"
#include "vgLib-2.0/sequencer/GateSequencer.hpp"

using namespace vgLib_v2;

#include "DigitalSequencerXP/defines.h"
#include "DigitalSequencerXP/DigitalSequencerXP.hpp"
#include "DigitalSequencerXP/SequencerDisplay.hpp"
#include "DigitalSequencerXP/VoltageSequencerDisplayXP.hpp"
#include "DigitalSequencerXP/GateSequencerDisplayXP.hpp"
#include "DigitalSequencerXP/DigitalSequencerXPWidget.hpp"

Model* modelDigitalSequencerXP = createModel<DigitalSequencerXP, DigitalSequencerXPWidget>("digitalsequencerxp");
