// ----------------------------------------------------------------------------
// Constants.cpp
//
//
// Authors:
// Peter Polidoro peterpolidoro@gmail.com
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
  .version_major=3,
  .version_minor=0,
  .version_patch=0,
};

const long ratio_value_default = 1;

CONSTANT_STRING(mix_volume_fill_duration_string,"mix_volume_fill_duration");
CONSTANT_STRING(mix_ratio_string,"mix_ratio");
CONSTANT_STRING(valve_open_durations_string,"valve_open_durations");

const long seconds_per_minute = 60;
const long milliseconds_per_second = 1000;

// Pins

// Units
CONSTANT_STRING(ml_units,"ml");
CONSTANT_STRING(ml_per_min_units,"ml/min");
CONSTANT_STRING(percent_units,"%");

// Properties
CONSTANT_STRING(valve_count_property_name,"valveCount");
const long valve_count_min = 2;
const long valve_count_max = VALVE_COUNT_MAX;
const long valve_count_default = 2;

CONSTANT_STRING(mix_volume_property_name,"mixVolume");
const long mix_volume_min = 2;
const long mix_volume_max = 8;
const long mix_volume_default = 6;

CONSTANT_STRING(mix_volume_limit_property_name,"mixVolumeLimit");
const long mix_volume_limit_min = 1;
const long mix_volume_limit_max = 100;
const long mix_volume_limit_default = 80;

CONSTANT_STRING(flow_rate_property_name,"flowRate");
const long flow_rate_min = 20;
const long flow_rate_max = 80;
const long flow_rate_default = 40;

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

CONSTANT_STRING(valve_open_duration_limit_property_name,"valveOpenDurationLimit");
const long valve_open_duration_limit_min = 10;
const long valve_open_duration_limit_max = 100;
const long valve_open_duration_limit_default = 20;

// Parameters
CONSTANT_STRING(ratio_parameter_name,"ratio");
const long ratio_min = 0;
const long ratio_max = mix_resolution_max;

CONSTANT_STRING(duration_parameter_name,"duration");
const long duration_min = 1;
const long duration_max = 2147483647;

// Functions
CONSTANT_STRING(get_mix_info_function_name,"getMixInfo");
CONSTANT_STRING(start_mixing_function_name,"startMixing");
CONSTANT_STRING(set_mix_duration_factored_function_name,"setMixDurationFactored");

// Callbacks
CONSTANT_STRING(stop_mixing_callback_name,"stopMixing");

// Errors
}
}
