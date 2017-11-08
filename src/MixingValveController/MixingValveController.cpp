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
  mixing_volume_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateMixTimingHandler));

  modular_server::Property & mixing_volume_limit_property = modular_server_.createProperty(constants::mixing_volume_limit_property_name,constants::mixing_volume_limit_default);
  mixing_volume_limit_property.setRange(constants::mixing_volume_limit_min,constants::mixing_volume_limit_max);
  mixing_volume_limit_property.setUnits(constants::percent_units);
  mixing_volume_limit_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateMixTimingHandler));

  modular_server::Property & flow_rate_property = modular_server_.createProperty(constants::flow_rate_property_name,constants::flow_rate_default);
  flow_rate_property.setRange(constants::flow_rate_min,constants::flow_rate_max);
  flow_rate_property.setUnits(constants::ml_per_min_units);
  flow_rate_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateMixTimingHandler));

  modular_server::Property & mix_resolution_property = modular_server_.createProperty(constants::mix_resolution_property_name,constants::mix_resolution_default);
  mix_resolution_property.setRange(constants::mix_resolution_min,constants::mix_resolution_max);
  mix_resolution_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateMixTimingHandler));

  modular_server::Property & mix_duration_property = modular_server_.createProperty(constants::mix_duration_property_name,constants::mix_duration_default);
  mix_duration_property.setRange(constants::mix_duration_min,constants::mix_duration_max);
  mix_duration_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateMixTimingHandler));

  modular_server::Property & valve_switch_duration_property = modular_server_.createProperty(constants::valve_switch_duration_property_name,constants::valve_switch_duration_default);
  valve_switch_duration_property.setRange(constants::valve_switch_duration_min,constants::valve_switch_duration_max);

  modular_server::Property & valve_open_unit_duration_min_property = modular_server_.createProperty(constants::valve_open_unit_duration_min_property_name,constants::valve_open_unit_duration_min_default);
  valve_open_unit_duration_min_property.setRange(constants::valve_open_unit_duration_min_min,constants::valve_open_unit_duration_min_max);
  valve_open_unit_duration_min_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateMixTimingHandler));

  // Parameters
  modular_server::Parameter & ratio_parameter = modular_server_.createParameter(constants::ratio_parameter_name);
  ratio_parameter.setRange(constants::ratio_min,constants::ratio_max);
  ratio_parameter.setTypeLong();
  ratio_parameter.setArrayLengthRange(constants::valve_count_min,constants::valve_count_max);

  setValveCountHandler();

  // Functions
  modular_server::Function & get_mix_timing_function = modular_server_.createFunction(constants::get_mix_timing_function_name);
  get_mix_timing_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::getMixTimingHandler));
  get_mix_timing_function.setResultTypeObject();

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
  Ratio mix_ratio = normalizeRatio(ratio);
  mix_info_.ratio = mix_ratio;
  mix_info_.valve = 0;
  mixHandler(constants::MIX_ARG_STARTED);
  return mix_ratio;
}

void MixingValveController::stopMixing()
{
  event_controller_.remove(mix_info_.event_id);
  setAllChannelsOff();
}

long MixingValveController::getMixingVolumeFillDuration()
{
  return mixing_volume_fill_duration_;
}

long MixingValveController::getValveOpenUnitDuration()
{
  return valve_open_unit_duration_;
}

bool MixingValveController::finishMix()
{
  bool continue_mixing = true;
  return continue_mixing;
}

MixingValveController::Ratio MixingValveController::normalizeRatio(MixingValveController::Ratio ratio)
{
  Ratio normalized_ratio(ratio);

  long mix_resolution;
  modular_server_.property(constants::mix_resolution_property_name).getValue(mix_resolution);

  size_t ratio_sum = ratioSum(normalized_ratio);
  if (ratio_sum == (size_t)mix_resolution)
  {
    return normalized_ratio;
  }
  else
  {
    long valve_count;
    modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

    for (size_t valve=0; valve<(size_t)valve_count; ++valve)
    {
      if (normalized_ratio[valve] > 0)
      {
        normalized_ratio[valve] = (normalized_ratio[valve]*mix_resolution)/ratio_sum;
        if (normalized_ratio[valve] < 1)
        {
          normalized_ratio[valve] = 1;
        }
      }
    }
  }

  ratio_sum = ratioSum(normalized_ratio);
  if (ratio_sum == (size_t)mix_resolution)
  {
    return normalized_ratio;
  }
  else
  {
    size_t ratio_max_index = ratioMaxIndex(normalized_ratio);
    normalized_ratio[ratio_max_index] = mix_resolution - (ratio_sum - normalized_ratio[ratio_max_index]);
  }
  return normalized_ratio;
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

size_t MixingValveController::ratioMaxIndex(MixingValveController::Ratio ratio)
{
  long valve_count;
  modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

  size_t ratio_max = 0;
  size_t ratio_max_index = 0;
  for (size_t valve=0; valve<(size_t)valve_count; ++valve)
  {
    if (ratio[valve] > ratio_max)
    {
      ratio_max = ratio[valve];
      ratio_max_index = valve;
    }
  }
  return ratio_max_index;
}

long MixingValveController::constrainMixDuration(const long mix_duration,
                                                 const long mix_duration_min,
                                                 const long mix_duration_max)
{
  long constrained_mix_duration = mix_duration;

  size_t inc = 1;
  while(constrained_mix_duration > mix_duration_max)
  {
    constrained_mix_duration = mix_duration/++inc;
  }
  if (constrained_mix_duration < mix_duration_min)
  {
    constrained_mix_duration = mix_duration_min;
  }
  return constrained_mix_duration;
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

  updateMixTimingHandler();
}

void MixingValveController::updateMixTimingHandler()
{
  long mixing_volume;
  modular_server_.property(constants::mixing_volume_property_name).getValue(mixing_volume);

  long mixing_volume_limit;
  modular_server_.property(constants::mixing_volume_limit_property_name).getValue(mixing_volume_limit);

  long flow_rate;
  modular_server_.property(constants::flow_rate_property_name).getValue(flow_rate);

  mixing_volume_fill_duration_ = (mixing_volume*constants::seconds_per_minute*constants::milliseconds_per_second)/flow_rate;
  mixing_duration_limit_ = (mixing_volume_fill_duration_*mixing_volume_limit)/constants::mixing_volume_limit_max;

  long valve_count;
  modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

  long valve_switch_duration;
  modular_server_.property(constants::valve_switch_duration_property_name).getValue(valve_switch_duration);

  long valve_open_unit_duration_min;
  modular_server_.property(constants::valve_open_unit_duration_min_property_name).getValue(valve_open_unit_duration_min);

  modular_server::Property & mix_resolution_property = modular_server_.property(constants::mix_resolution_property_name);
  mix_resolution_property.disableFunctors();
  mix_resolution_property.setDefaultValue(constants::mix_resolution_default);
  long mix_resolution_min = valve_count*2;
  long mix_resolution_max = (mixing_duration_limit_ - valve_count*valve_switch_duration)/valve_open_unit_duration_min;
  mix_resolution_property.setRange(mix_resolution_min,mix_resolution_max);
  mix_resolution_property.reenableFunctors();
  long mix_resolution;
  mix_resolution_property.getValue(mix_resolution);

  modular_server::Parameter & ratio_parameter = modular_server_.parameter(constants::ratio_parameter_name);
  ratio_parameter.setRange(constants::ratio_min,mix_resolution);

  modular_server::Property & mix_duration_property = modular_server_.property(constants::mix_duration_property_name);
  mix_duration_property.disableFunctors();
  mix_duration_property.setDefaultValue(constants::mix_duration_default);
  long mix_duration_min = valve_open_unit_duration_min*mix_resolution + valve_count*valve_switch_duration;
  long mix_duration_max = mixing_duration_limit_ + valve_count*valve_switch_duration;
  long mix_duration;
  mix_duration_property.getValue(mix_duration);

  long constrained_mix_duration = constrainMixDuration(mix_duration,
                                                       mix_duration_min,
                                                       mix_duration_max);

  valve_open_unit_duration_ = (constrained_mix_duration - valve_count*valve_switch_duration)/mix_resolution;

  mix_duration = valve_open_unit_duration_*mix_resolution + valve_count*valve_switch_duration;
  mix_duration_property.setValue(mix_duration);
  mix_duration_property.reenableFunctors();
}

void MixingValveController::getMixTimingHandler()
{
  modular_server_.response().writeResultKey();

  modular_server_.response().beginObject();

  modular_server_.response().write(constants::mixing_volume_fill_duration_string,mixing_volume_fill_duration_);
  modular_server_.response().write(constants::mixing_duration_limit_string,mixing_duration_limit_);
  modular_server_.response().write(constants::valve_open_unit_duration_string,valve_open_unit_duration_);

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

void MixingValveController::mixHandler(int arg)
{
  bool continue_mixing = true;

  if (arg == constants::MIX_ARG_FINISHED)
  {
    continue_mixing = finishMix();
  }

  if (continue_mixing && event_controller_.eventsAvailable())
  {
    setChannelOnAllOthersOff(mix_info_.valve);

    long valve_switch_duration;
    modular_server_.property(constants::valve_switch_duration_property_name).getValue(valve_switch_duration);

    long valve_on_duration = (mix_info_.ratio[mix_info_.valve]*valve_open_unit_duration_) + valve_switch_duration;

    long valve_count;
    modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

    mix_info_.valve = (mix_info_.valve + 1) % valve_count;

    int mix_arg = constants::MIX_ARG_NORMAL;

    if (mix_info_.valve == 0)
    {
      mix_arg = constants::MIX_ARG_FINISHED;
    }

    mix_info_.event_id = event_controller_.addEventUsingDelay(makeFunctor((Functor1<int> *)0,*this,&MixingValveController::mixHandler),
                                                              valve_on_duration,
                                                              mix_arg);

    event_controller_.enable(mix_info_.event_id);
  }
}
