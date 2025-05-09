//
// Voxglitch "wav bank" module for VCV Rack
//
// This code is heavily based on Clément Foulc's PLAY module
// which can be found here:  https://github.com/cfoulc/cf/blob/v1/src/PLAY.cpp

#include "plugin.hpp"
#ifndef METAMODULE
#include "osdialog.h"
#else
#include "async_filebrowser.hh"
#endif

#include "vgLib-2.0/constants.h"
#include "vgLib-2.0/sample.hpp"
#include "vgLib-2.0/dsp/DeclickFilter.hpp"

#include "vgLib-2.0/components/VoxglitchComponents.hpp"
#include "vgLib-2.0/SamplePlayer.hpp"

using namespace vgLib_v2;

#include "WavBank/defines.h"
#include "WavBank/WavBank.hpp"
#include "WavBank/WavBankReadout.hpp"
#include "WavBank/MenuItemLoadBank.hpp"

#include "WavBank/WavBankWidget.hpp"

Model* modelWavBank = createModel<WavBank, WavBankWidget>("wavbank");
