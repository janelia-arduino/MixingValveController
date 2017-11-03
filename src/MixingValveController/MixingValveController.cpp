// ----------------------------------------------------------------------------
// MixingValveController.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "../MixingValveController.h"


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
  valve_count_property.setRange(constants::valve_count_min,(long)constants::VALVE_COUNT_MAX);
  valve_count_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::setValveCountHandler));

  modular_server::Property & mixing_volume_property = modular_server_.createProperty(constants::mixing_volume_property_name,constants::mixing_volume_default);
  mixing_volume_property.setRange(constants::mixing_volume_min,constants::mixing_volume_max);
  mixing_volume_property.setUnits(constants::ml_units);
  mixing_volume_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateTimingHandler));

  modular_server::Property & flow_rate_property = modular_server_.createProperty(constants::flow_rate_property_name,constants::flow_rate_default);
  flow_rate_property.setRange(constants::flow_rate_min,constants::flow_rate_max);
  flow_rate_property.setUnits(constants::ml_per_min_units);
  flow_rate_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateTimingHandler));

  modular_server::Property & resolution_property = modular_server_.createProperty(constants::resolution_property_name,constants::resolution_default);
  resolution_property.setRange(constants::resolution_min,constants::resolution_max);
  resolution_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateTimingHandler));

  modular_server::Property & valve_switch_duration_property = modular_server_.createProperty(constants::valve_switch_duration_property_name,constants::valve_switch_duration_default);
  valve_switch_duration_property.setRange(constants::valve_switch_duration_min,constants::valve_switch_duration_max);

  // Parameters
  modular_server::Parameter & ratio_parameter = modular_server_.createParameter(constants::ratio_parameter_name);
  ratio_parameter.setRange(constants::ratio_min,constants::ratio_max);
  ratio_parameter.setTypeLong();
  ratio_parameter.setArrayLengthRange(constants::valve_count_min,constants::valve_count_max);

  setValveCountHandler();
  updateTimingHandler();

  // Functions
  modular_server::Function & get_timing_function = modular_server_.createFunction(constants::get_timing_function_name);
  get_timing_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::getTimingHandler));
  get_timing_function.setResultTypeObject();

  modular_server::Function & start_mixing_function = modular_server_.createFunction(constants::start_mixing_function_name);
  start_mixing_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::startMixingHandler));
  start_mixing_function.addParameter(ratio_parameter);
  start_mixing_function.setResultTypeLong();
  start_mixing_function.setResultTypeArray();

  // Callbacks
  modular_server::Callback & stop_mixing_callback = modular_server_.createCallback(constants::stop_mixing_callback_name);
  stop_mixing_callback.attachFunctor(makeFunctor((Functor1<modular_server::Interrupt *> *)0,*this,&MixingValveController::stopMixingHandler));

}

MixingValveController::Ratio MixingValveController::startMixing(MixingValveController::Ratio ratio)
{
  stopMixing();
  Ratio mix_ratio = constrainRatio(ratio);
  mix_info_.ratio = mix_ratio;
  mix_info_.valve = 0;
  mixHandler(-1);
  return mix_ratio;
}

void MixingValveController::stopMixing()
{
  event_controller_.remove(mix_info_.event_id);
  setAllChannelsOff();
}

MixingValveController::Ratio MixingValveController::constrainRatio(MixingValveController::Ratio ratio)
{
  Ratio constrained_ratio(ratio);

  size_t ratio_sum = ratioSum(constrained_ratio);

  long resolution;
  modular_server_.property(constants::resolution_property_name).getValue(resolution);

  if (ratio_sum <= (size_t)resolution)
  {
    return constrained_ratio;
  }
  else
  {
    long valve_count;
    modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

    for (size_t valve=0; valve<(size_t)valve_count; ++valve)
    {
      if (constrained_ratio[valve] > 0)
      {
        constrained_ratio[valve] = (constrained_ratio[valve]*resolution)/ratio_sum;
        if (constrained_ratio[valve] < 1)
        {
          constrained_ratio[valve] = 1;
        }
      }
    }
  }
  return constrainRatio(constrained_ratio);
}

size_t MixingValveController::ratioSum(MixingValveController::Ratio ratio)
{
  long valve_count;
  modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

  size_t ratio_sum = 0;
  for (size_t valve=0; valve<(size_t)valve_count; ++valve)
  {
    ratio_sum += ratio[valve];
  }
  return ratio_sum;
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

void MixingValveController::setValveCountHandler()
{
  long valve_count;
  modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

  modular_server::Parameter & ratio_parameter = modular_server_.parameter(constants::ratio_parameter_name);
  ratio_parameter.setArrayLengthRange(valve_count,valve_count);
}

void MixingValveController::updateTimingHandler()
{
  long mixing_volume;
  modular_server_.property(constants::mixing_volume_property_name).getValue(mixing_volume);

  long flow_rate;
  modular_server_.property(constants::flow_rate_property_name).getValue(flow_rate);

  long resolution;
  modular_server_.property(constants::resolution_property_name).getValue(resolution);

  mixing_volume_fill_duration_ = (mixing_volume*constants::seconds_per_minute*constants::milliseconds_per_second)/flow_rate;
  valve_open_duration_min_ = mixing_volume_fill_duration_/resolution;
  if (valve_open_duration_min_ < 1)
  {
    valve_open_duration_min_ = 1;
  }

  modular_server::Parameter & ratio_parameter = modular_server_.parameter(constants::ratio_parameter_name);
  ratio_parameter.setRange(constants::ratio_min,resolution);
}

void MixingValveController::getTimingHandler()
{
  modular_server_.response().writeResultKey();

  modular_server_.response().beginObject();

  modular_server_.response().write(constants::mixing_volume_fill_duration_string,mixing_volume_fill_duration_);
  modular_server_.response().write(constants::valve_open_duration_min_string,valve_open_duration_min_);

  modular_server_.response().endObject();
}

void MixingValveController::startMixingHandler()
{
  ArduinoJson::JsonArray * ratio_ptr;
  modular_server_.parameter(constants::ratio_parameter_name).getValue(ratio_ptr);

  long valve_count;
  modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

  Ratio mix_ratio;
  for (size_t valve=0; valve<(size_t)valve_count; ++valve)
  {
    mix_ratio.push_back((*ratio_ptr)[valve]);
  }

  mix_ratio = startMixing(mix_ratio);

  modular_server_.response().returnResult(mix_ratio);
}

void MixingValveController::stopMixingHandler(modular_server::Interrupt * interrupt_ptr)
{
  stopMixing();
}

void MixingValveController::mixHandler(int index)
{
  if (event_controller_.eventsAvailable())
  {
    setChannelOnAllOthersOff(mix_info_.valve);

    long valve_switch_duration;
    modular_server_.property(constants::valve_switch_duration_property_name).getValue(valve_switch_duration);

    long valve_on_duration = (mix_info_.ratio[mix_info_.valve]*valve_open_duration_min_) + valve_switch_duration;

    mix_info_.event_id = event_controller_.addEventUsingDelay(makeFunctor((Functor1<int> *)0,*this,&MixingValveController::mixHandler),
                                                              valve_on_duration);

    long valve_count;
    modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

    mix_info_.valve = (mix_info_.valve + 1) % valve_count;

    event_controller_.enable(mix_info_.event_id);
  }
}
