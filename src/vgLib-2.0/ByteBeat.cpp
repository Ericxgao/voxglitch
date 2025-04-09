//
// Voxglitch "SamplerX8" module for VCV Rack
//

#include "plugin.hpp"
#ifndef METAMODULE
#include "osdialog.h"
#else
#include "async_filebrowser.hh"
#endif

// #include "ByteBeat/calculator.hpp"
#include "ByteBeat/defines.h"
#include "ByteBeat/ByteBeat.hpp"
#include "ByteBeat/ByteBeatWidget.hpp"

Model* modelByteBeat = createModel<ByteBeat, ByteBeatWidget>("bytebeat");
