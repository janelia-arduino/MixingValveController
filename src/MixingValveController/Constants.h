// ----------------------------------------------------------------------------
// Constants.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef MIXING_VALVE_CONTROLLER_CONSTANTS_H
#define MIXING_VALVE_CONTROLLER_CONSTANTS_H
#include <ConstantVariable.h>
#include <PowerSwitchController.h>


namespace mixing_valve_controller
{
namespace constants
{
//MAX values must be >= 1, >= created/copied count, < RAM limit
enum{PROPERTY_COUNT_MAX=7};
enum{PARAMETER_COUNT_MAX=1};
enum{FUNCTION_COUNT_MAX=2};
enum{CALLBACK_COUNT_MAX=1};

enum{VALVE_COUNT_MAX=power_switch_controller::constants::CHANNEL_COUNT};
enum{MIX_COUNT_MAX=1};

extern ConstantString device_name;

extern ConstantString firmware_name;
extern const modular_server::FirmwareInfo firmware_info;

struct MixInfo
{
  Array<size_t,VALVE_COUNT_MAX> ratio;
  EventId event_id;
  size_t valve;
};

extern ConstantString mixing_volume_fill_duration_string;
extern ConstantString valve_open_unit_duration_string;

extern const long seconds_per_minute;
extern const long milliseconds_per_second;

// Interrupts

// Units
extern ConstantString ml_units;
extern ConstantString ml_per_min_units;

// Properties
// Property values must be long, double, bool, long[], double[], bool[], char[], ConstantString *, (ConstantString *)[]
extern ConstantString valve_count_property_name;
extern const long valve_count_min;
extern const long valve_count_max;
extern const long valve_count_default;

extern ConstantString mixing_volume_property_name;
extern const long mixing_volume_min;
extern const long mixing_volume_max;
extern const long mixing_volume_default;

extern ConstantString flow_rate_property_name;
extern const long flow_rate_min;
extern const long flow_rate_max;
extern const long flow_rate_default;

extern ConstantString mix_resolution_property_name;
extern const long mix_resolution_min;
extern const long mix_resolution_max;
extern const long mix_resolution_default;

extern ConstantString mix_duration_property_name;
extern const long mix_duration_min;
extern const long mix_duration_max;
extern const long mix_duration_default;

extern ConstantString valve_switch_duration_property_name;
extern const long valve_switch_duration_min;
extern const long valve_switch_duration_max;
extern const long valve_switch_duration_default;

extern ConstantString valve_open_unit_duration_min_property_name;
extern const long valve_open_unit_duration_min_min;
extern const long valve_open_unit_duration_min_max;
extern const long valve_open_unit_duration_min_default;

// Parameters
extern ConstantString ratio_parameter_name;
extern const long ratio_min;
extern const long ratio_max;

// Functions
extern ConstantString get_mix_timing_function_name;
extern ConstantString start_mixing_function_name;

// Callbacks
extern ConstantString stop_mixing_callback_name;

// Errors
}
}
#include "5x3.h"
#include "3x2.h"
#endif
