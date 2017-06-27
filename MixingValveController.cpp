// ----------------------------------------------------------------------------
// MixingValveController.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "MixingValveController.h"


using namespace mixing_valve_controller;

MixingValveController::MixingValveController()
{
}

void MixingValveController::setup()
{
  // Parent Setup
  PowerSwitchController::setup();

  // Pin Setup

  // Set Device ID
  modular_server_.setDeviceName(constants::device_name);

  // Add Hardware

  // Interrupts

  // Add Firmware
  modular_server_.addFirmware(constants::firmware_info,
                              properties_,
                              parameters_,
                              functions_,
                              callbacks_);

  // Properties
  modular_server::Property & valve_count_property = modular_server_.createProperty(constants::valve_count_property_name,constants::valve_count_default);
  valve_count_property.setRange((long)1,(long)constants::VALVE_COUNT_MAX);

  modular_server::Property & mix_unit_duration_property = modular_server_.createProperty(constants::mix_unit_duration_property_name,constants::mix_unit_duration_default);
  mix_unit_duration_property.setRange(constants::mix_unit_duration_min,constants::mix_unit_duration_max);

  // Parameters

  // Functions
  modular_server::Function & mix_function = modular_server_.createFunction(constants::mix_function_name);
  mix_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::mixHandler));
  mix_function.setResultTypeLong();

  // Callbacks

  mixHandler();

}

int MixingValveController::mix(Array<size_t,mixing_valve_controller::constants::VALVE_COUNT_MAX> ratio)
{
  if (!event_controller_.eventsAvailable())
  {
    return power_switch_controller::constants::bad_index;
  }

  long valve_count;
  modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

  long mix_unit_duration;
  modular_server_.property(constants::mix_unit_duration_property_name).getValue(mix_unit_duration);

  constants::MixInfo mix_info;
  mix_info.ratio = ratio;
  mix_info.valve = 0;
  mix_info.count = ratio[0];
  mix_info.inc = 0;
  indexed_mix_info_.clear();

  int index = indexed_mix_info_.add(mix_info);

  EventId event_id = event_controller_.addInfiniteRecurringEventUsingDelay(makeFunctor((Functor1<int> *)0,*this,&MixingValveController::mixHandler),
                                                                           mix_unit_duration,
                                                                           mix_unit_duration,
                                                                           index);
  indexed_mix_info_[index].event_id = event_id;
  event_controller_.enable(event_id);
  return index;
}

// Handlers must be non-blocking (avoid 'delay')
//
// modular_server_.parameter(parameter_name).getValue(value) value type must be either:
// fixed-point number (int, long, etc.)
// floating-point number (float, double)
// bool
// const char *
// ArduinoJson::JsonArray *
// ArduinoJson::JsonObject *
//
// For more info read about ArduinoJson parsing https://github.com/janelia-arduino/ArduinoJson
//
// modular_server_.property(property_name).getValue(value) value type must match the property default type
// modular_server_.property(property_name).setValue(value) value type must match the property default type
// modular_server_.property(property_name).getElementValue(element_index,value) value type must match the property array element default type
// modular_server_.property(property_name).setElementValue(element_index,value) value type must match the property array element default type

void MixingValveController::mixHandler()
{
  Array<size_t,constants::VALVE_COUNT_MAX> mix_ratio;
  mix_ratio.push_back(3);
  mix_ratio.push_back(5);
  int index = mix(mix_ratio);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(power_switch_controller::constants::pwm_error);
  }
}

void MixingValveController::mixHandler(int index)
{
  long valve_count;
  modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

  constants::MixInfo & mix_info = indexed_mix_info_[index];
  if (mix_info.inc == mix_info.count)
  {
    mix_info.valve = (mix_info.valve + 1) % valve_count;
    mix_info.count = mix_info.ratio[mix_info.valve];
    mix_info.inc = 0;
  }
  if (mix_info.inc == 0)
  {
    setChannelOnAllOthersOff(mix_info.valve);
    // Serial << "turning on " << mix_info.valve << "\n";
  }
  // Serial << "inc " << mix_info.inc << "\n";
  ++mix_info.inc;
}
