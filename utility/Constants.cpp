// ----------------------------------------------------------------------------
// Constants.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Constants.h"


namespace mixing_valve_controller
{
namespace constants
{
CONSTANT_STRING(device_name,"mixing_valve_controller");

CONSTANT_STRING(firmware_name,"MixingValveController");
// Use semantic versioning http://semver.org/
const modular_server::FirmwareInfo firmware_info =
  {
    .name_ptr=&firmware_name,
    .version_major=1,
    .version_minor=0,
    .version_patch=0,
  };

// Interrupts

// Units

// Properties
CONSTANT_STRING(valve_count_property_name,"valveCount");
const long valve_count_default = 2;

CONSTANT_STRING(mix_unit_duration_property_name,"mixUnitDuration");
const long mix_unit_duration_min = 50;
const long mix_unit_duration_max = 1000;
const long mix_unit_duration_default = 1000;

// Parameters

// Functions
CONSTANT_STRING(mix_function_name,"mix");

// Callbacks

// Errors
}
}
