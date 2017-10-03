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
enum{PROPERTY_COUNT_MAX=2};
enum{PARAMETER_COUNT_MAX=1};
enum{FUNCTION_COUNT_MAX=1};
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
  uint8_t valve;
  size_t count;
  size_t inc;
};

// Interrupts

// Units

// Properties
// Property values must be long, double, bool, long[], double[], bool[], char[], ConstantString *, (ConstantString *)[]
extern ConstantString valve_count_property_name;
extern const long valve_count_default;

extern ConstantString mix_unit_duration_property_name;
extern const long mix_unit_duration_min;
extern const long mix_unit_duration_max;
extern const long mix_unit_duration_default;

// Parameters

// Functions
extern ConstantString mix_function_name;

// Callbacks

// Errors
}
}
#include "5x3.h"
#include "3x2.h"
#endif
