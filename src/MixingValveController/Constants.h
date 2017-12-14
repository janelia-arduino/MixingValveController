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
enum{PROPERTY_COUNT_MAX=8};
enum{PARAMETER_COUNT_MAX=2};
enum{FUNCTION_COUNT_MAX=3};
enum{CALLBACK_COUNT_MAX=1};

enum{VALVE_COUNT_MAX=power_switch_controller::constants::CHANNEL_COUNT};
enum{MIX_COUNT_MAX=1};

enum{MIX_ARG_STARTED=-1};
enum{MIX_ARG_NORMAL=0};
enum{MIX_ARG_FINISHED=1};

extern ConstantString device_name;

extern ConstantString firmware_name;
extern const modular_server::FirmwareInfo firmware_info;

struct MixInfo
{
  Array<long,VALVE_COUNT_MAX> ratio;
  Array<long,VALVE_COUNT_MAX> valve_open_durations;
  EventId event_id;
  size_t valve;
};

extern const long ratio_value_default;

extern ConstantString mix_volume_fill_duration_string;
extern ConstantString mix_ratio_string;
extern ConstantString valve_open_durations_string;

extern const long seconds_per_minute;
extern const long milliseconds_per_second;

// Interrupts

// Units
extern ConstantString ml_units;
extern ConstantString ml_per_min_units;
extern ConstantString percent_units;

// Properties
// Property values must be long, double, bool, long[], double[], bool[], char[], ConstantString *, (ConstantString *)[]
extern ConstantString valve_count_property_name;
extern const long valve_count_min;
extern const long valve_count_max;
extern const long valve_count_default;

extern ConstantString mix_volume_property_name;
extern const long mix_volume_min;
extern const long mix_volume_max;
extern const long mix_volume_default;

extern ConstantString mix_volume_limit_property_name;
extern const long mix_volume_limit_min;
extern const long mix_volume_limit_max;
extern const long mix_volume_limit_default;

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

extern ConstantString valve_open_duration_limit_property_name;
extern const long valve_open_duration_limit_min;
extern const long valve_open_duration_limit_max;
extern const long valve_open_duration_limit_default;

// Parameters
extern ConstantString ratio_parameter_name;
extern const long ratio_min;
extern const long ratio_max;

extern ConstantString duration_parameter_name;
extern const long duration_min;
extern const long duration_max;

// Functions
extern ConstantString get_mix_info_function_name;
extern ConstantString start_mixing_function_name;
extern ConstantString set_mix_duration_factored_function_name;

// Callbacks
extern ConstantString stop_mixing_callback_name;

// Errors
}
}
#include "5x3.h"
#include "3x2.h"
#endif
