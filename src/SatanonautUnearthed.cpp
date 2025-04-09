//
// Voxglitch "SatanonautUnearthed" module for VCV Rack
//

#include "plugin.hpp"
#ifndef METAMODULE
#include "osdialog.h"
#else
#include "async_filebrowser.hh"
#endif

#include "vgLib-2.0/constants.h"
#include "vgLib-2.0/components/VoxglitchComponents.hpp"

using namespace vgLib_v2;

// #include "ByteBeat/calculator.hpp"
#include "SatanonautUnearthed/SatanonautUnearthed.hpp"
#include "SatanonautUnearthed/SatanonautUnearthedWidget.hpp"

Model* modelSatanonautUnearthed = createModel<SatanonautUnearthed, SatanonautUnearthedWidget>("satanonaut_unearthed");
