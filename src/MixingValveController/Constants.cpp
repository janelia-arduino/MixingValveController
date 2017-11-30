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
    .version_patch=2,
  };

CONSTANT_STRING(mixing_volume_fill_duration_string,"mixing_volume_fill_duration");
CONSTANT_STRING(mixing_duration_limit_string,"mixing_duration_limit");
CONSTANT_STRING(valve_open_unit_duration_string,"valve_open_unit_duration");

const long seconds_per_minute = 60;
const long milliseconds_per_second = 1000;

// Interrupts

// Units
CONSTANT_STRING(ml_units,"ml");
CONSTANT_STRING(ml_per_min_units,"ml/min");
CONSTANT_STRING(percent_units,"%");

// Properties
CONSTANT_STRING(valve_count_property_name,"valveCount");
const long valve_count_min = 2;
const long valve_count_max = VALVE_COUNT_MAX;
const long valve_count_default = 2;

CONSTANT_STRING(mixing_volume_property_name,"mixingVolume");
const long mixing_volume_min = 2;
const long mixing_volume_max = 8;
const long mixing_volume_default = 6;

CONSTANT_STRING(mixing_volume_limit_property_name,"mixingVolumeLimit");
const long mixing_volume_limit_min = 1;
const long mixing_volume_limit_max = 100;
const long mixing_volume_limit_default = 80;

CONSTANT_STRING(flow_rate_property_name,"flowRate");
const long flow_rate_min = 20;
const long flow_rate_max = 80;
const long flow_rate_default = 42;

CONSTANT_STRING(mix_resolution_property_name,"mixResolution");
const long mix_resolution_min = VALVE_COUNT_MAX*2;
const long mix_resolution_max = 10000;
const long mix_resolution_default = 100;

CONSTANT_STRING(mix_duration_property_name,"mixDuration");
const long mix_duration_min = 1;
const long mix_duration_max = 3600000;
const long mix_duration_default = 6000;

CONSTANT_STRING(valve_switch_duration_property_name,"valveSwitchDuration");
const long valve_switch_duration_min = 10;
const long valve_switch_duration_max = 100;
const long valve_switch_duration_default = 30;

CONSTANT_STRING(valve_open_unit_duration_min_property_name,"valveOpenUnitDurationMin");
const long valve_open_unit_duration_min_min = 1;
const long valve_open_unit_duration_min_max = 1000;
const long valve_open_unit_duration_min_default = 30;

// Parameters
CONSTANT_STRING(ratio_parameter_name,"ratio");
const long ratio_min = 0;
const long ratio_max = mix_resolution_max;

// Functions
CONSTANT_STRING(get_mix_timing_function_name,"getMixTiming");
CONSTANT_STRING(start_mixing_function_name,"startMixing");

// Callbacks
CONSTANT_STRING(stop_mixing_callback_name,"stopMixing");

// Errors
}
}
