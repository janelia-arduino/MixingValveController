// ----------------------------------------------------------------------------
// MixingValveController.cpp
//
//
// Authors:
// Peter Polidoro peterpolidoro@gmail.com
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

  // Reset Watchdog
  resetWatchdog();

  // Pin Setup

  // Set Device ID
  modular_server_.setDeviceName(constants::device_name);

  // Add Hardware

  // Pins

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

  modular_server::Property & mix_volume_property = modular_server_.createProperty(constants::mix_volume_property_name,constants::mix_volume_default);
  mix_volume_property.setRange(constants::mix_volume_min,constants::mix_volume_max);
  mix_volume_property.setUnits(constants::ml_units);
  mix_volume_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateMixPropertiesHandler));

  modular_server::Property & mix_volume_limit_property = modular_server_.createProperty(constants::mix_volume_limit_property_name,constants::mix_volume_limit_default);
  mix_volume_limit_property.setRange(constants::mix_volume_limit_min,constants::mix_volume_limit_max);
  mix_volume_limit_property.setUnits(constants::percent_units);
  mix_volume_limit_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateMixPropertiesHandler));

  modular_server::Property & flow_rate_property = modular_server_.createProperty(constants::flow_rate_property_name,constants::flow_rate_default);
  flow_rate_property.setRange(constants::flow_rate_min,constants::flow_rate_max);
  flow_rate_property.setUnits(constants::ml_per_min_units);
  flow_rate_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateMixPropertiesHandler));

  modular_server::Property & mix_resolution_property = modular_server_.createProperty(constants::mix_resolution_property_name,constants::mix_resolution_default);
  mix_resolution_property.setRange(constants::mix_resolution_min,constants::mix_resolution_max);
  mix_resolution_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateMixPropertiesHandler));

  modular_server::Property & mix_duration_property = modular_server_.createProperty(constants::mix_duration_property_name,constants::mix_duration_default);
  mix_duration_property.setRange(constants::mix_duration_min,constants::mix_duration_max);
  mix_duration_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateMixPropertiesHandler));

  modular_server::Property & valve_switch_duration_property = modular_server_.createProperty(constants::valve_switch_duration_property_name,constants::valve_switch_duration_default);
  valve_switch_duration_property.setRange(constants::valve_switch_duration_min,constants::valve_switch_duration_max);

  modular_server::Property & valve_open_duration_limit_property = modular_server_.createProperty(constants::valve_open_duration_limit_property_name,constants::valve_open_duration_limit_default);
  valve_open_duration_limit_property.setRange(constants::valve_open_duration_limit_min,constants::valve_open_duration_limit_max);
  valve_open_duration_limit_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::updateMixPropertiesHandler));

  // Parameters
  modular_server::Parameter & ratio_parameter = modular_server_.createParameter(constants::ratio_parameter_name);
  ratio_parameter.setRange(constants::ratio_min,constants::ratio_max);
  ratio_parameter.setTypeLong();
  ratio_parameter.setArrayLengthRange(constants::valve_count_min,constants::valve_count_max);

  modular_server::Parameter & duration_parameter = modular_server_.createParameter(constants::duration_parameter_name);
  duration_parameter.setRange(constants::duration_min,constants::duration_max);
  duration_parameter.setUnits(power_switch_controller::constants::ms_units);

  setValveCountHandler();

  // Functions
  modular_server::Function & get_mix_info_function = modular_server_.createFunction(constants::get_mix_info_function_name);
  get_mix_info_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::getMixInfoHandler));
  get_mix_info_function.setResultTypeObject();

  modular_server::Function & start_mixing_function = modular_server_.createFunction(constants::start_mixing_function_name);
  start_mixing_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::startMixingHandler));
  start_mixing_function.addParameter(ratio_parameter);
  start_mixing_function.setResultTypeLong();
  start_mixing_function.setResultTypeArray();

  modular_server::Function & set_mix_duration_factored_function = modular_server_.createFunction(constants::set_mix_duration_factored_function_name);
  set_mix_duration_factored_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&MixingValveController::setMixDurationFactoredHandler));
  set_mix_duration_factored_function.addParameter(duration_parameter);
  set_mix_duration_factored_function.setResultTypeLong();

  // Callbacks
  modular_server::Callback & stop_mixing_callback = modular_server_.createCallback(constants::stop_mixing_callback_name);
  stop_mixing_callback.attachFunctor(makeFunctor((Functor1<modular_server::Pin *> *)0,*this,&MixingValveController::stopMixingHandler));

}

MixingValveController::ValveValues MixingValveController::startMixing(MixingValveController::ValveValues ratio)
{
  stopMixing();
  setMixInfo(ratio);
  mixHandler(constants::MIX_ARG_STARTED);
  return mix_info_.ratio;
}

void MixingValveController::stopMixing()
{
  event_controller_.remove(mix_info_.event_id);
  setAllChannelsOff();
}

long MixingValveController::getMixVolumeFillDuration()
{
  return mix_volume_fill_duration_;
}

long MixingValveController::setMixDurationFactored(const long duration)
{
  long mix_duration = constrainMixDuration(duration,
    mix_duration_min_,
    mix_duration_max_);

  modular_server::Property & mix_duration_property = modular_server_.property(constants::mix_duration_property_name);
  mix_duration_property.setValue(mix_duration);

  return mix_duration;
}

bool MixingValveController::finishMix()
{
  bool continue_mixing = true;
  return continue_mixing;
}

MixingValveController::ValveValues MixingValveController::normalizeRatio(MixingValveController::ValveValues ratio)
{
  ValveValues normalized_ratio(ratio);

  long mix_resolution;
  modular_server_.property(constants::mix_resolution_property_name).getValue(mix_resolution);

  long ratio_sum = valveValuesSum(normalized_ratio);
  if (ratio_sum == mix_resolution)
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

  ratio_sum = valveValuesSum(normalized_ratio);
  if (ratio_sum == mix_resolution)
  {
    return normalized_ratio;
  }
  else
  {
    size_t ratio_max_index = valveValuesMaxIndex(normalized_ratio);
    normalized_ratio[ratio_max_index] = mix_resolution - (ratio_sum - normalized_ratio[ratio_max_index]);
  }
  return normalized_ratio;
}

long MixingValveController::valveValuesSum(MixingValveController::ValveValues valve_values)
{
  long valve_count;
  modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

  long valve_values_sum = 0;
  for (size_t valve=0; valve<(size_t)valve_count; ++valve)
  {
    valve_values_sum += valve_values[valve];
  }
  return valve_values_sum;
}

size_t MixingValveController::valveValuesMaxIndex(MixingValveController::ValveValues valve_values)
{
  long valve_count;
  modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

  long valve_values_max = 0;
  size_t valve_values_max_index = 0;
  for (long valve=0; valve<valve_count; ++valve)
  {
    if (valve_values[valve] > valve_values_max)
    {
      valve_values_max = valve_values[valve];
      valve_values_max_index = valve;
    }
  }
  return valve_values_max_index;
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

void MixingValveController::setMixInfo(MixingValveController::ValveValues ratio)
{
  mix_info_.ratio = normalizeRatio(ratio);
  mix_info_.valve = 0;

  long valve_count;
  modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

  long mix_duration;
  modular_server_.property(constants::mix_duration_property_name).getValue(mix_duration);

  long valve_switch_duration;
  modular_server_.property(constants::valve_switch_duration_property_name).getValue(valve_switch_duration);

  long valve_open_duration_total = mix_duration - valve_switch_duration*valve_count;

  long ratio_sum = valveValuesSum(mix_info_.ratio);

  ValveValues valve_open_durations;
  for (long valve=0; valve<valve_count; ++valve)
  {
    long valve_open_duration = (valve_open_duration_total*mix_info_.ratio[valve])/ratio_sum;
    valve_open_durations.push_back(valve_open_duration);
  }

  long valve_open_durations_sum = valveValuesSum(valve_open_durations);
  size_t valve_open_durations_max_index = valveValuesMaxIndex(valve_open_durations);
  valve_open_durations[valve_open_durations_max_index] += valve_open_duration_total - valve_open_durations_sum;

  mix_info_.valve_open_durations = valve_open_durations;
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
// const ConstantString *
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

  updateMixPropertiesHandler();
}

void MixingValveController::updateMixPropertiesHandler()
{
  long mix_volume;
  modular_server_.property(constants::mix_volume_property_name).getValue(mix_volume);

  long mix_volume_limit;
  modular_server_.property(constants::mix_volume_limit_property_name).getValue(mix_volume_limit);

  long flow_rate;
  modular_server_.property(constants::flow_rate_property_name).getValue(flow_rate);

  mix_volume_fill_duration_ = (mix_volume*constants::seconds_per_minute*constants::milliseconds_per_second)/flow_rate;
  long mix_volume_fill_duration_limit = (mix_volume_fill_duration_*mix_volume_limit)/constants::mix_volume_limit_max;

  long valve_count;
  modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

  long valve_switch_duration;
  modular_server_.property(constants::valve_switch_duration_property_name).getValue(valve_switch_duration);

  long valve_open_duration_limit;
  modular_server_.property(constants::valve_open_duration_limit_property_name).getValue(valve_open_duration_limit);

  modular_server::Property & mix_resolution_property = modular_server_.property(constants::mix_resolution_property_name);
  mix_resolution_property.disableFunctors();
  long mix_resolution;
  mix_resolution_property.getValue(mix_resolution);
  long mix_resolution_min = valve_count*2;
  long mix_resolution_max = (mix_volume_fill_duration_limit - valve_count*valve_switch_duration)/valve_open_duration_limit;
  mix_resolution_property.setRange(mix_resolution_min,mix_resolution_max);
  mix_resolution_property.getValue(mix_resolution);
  mix_resolution_property.reenableFunctors();

  modular_server::Parameter & ratio_parameter = modular_server_.parameter(constants::ratio_parameter_name);
  ratio_parameter.setRange(constants::ratio_min,mix_resolution);

  modular_server::Property & mix_duration_property = modular_server_.property(constants::mix_duration_property_name);
  mix_duration_property.disableFunctors();
  mix_duration_min_ = valve_open_duration_limit*mix_resolution + valve_count*valve_switch_duration;
  mix_duration_max_ = mix_volume_fill_duration_limit + valve_count*valve_switch_duration;
  mix_duration_property.setRange(mix_duration_min_,mix_duration_max_);
  mix_duration_property.reenableFunctors();

}

void MixingValveController::getMixInfoHandler()
{
  modular_server_.response().writeResultKey();

  modular_server_.response().beginObject();

  modular_server_.response().write(constants::mix_volume_fill_duration_string,mix_volume_fill_duration_);
  modular_server_.response().write(constants::mix_ratio_string,mix_info_.ratio);
  modular_server_.response().write(constants::valve_open_durations_string,mix_info_.valve_open_durations);

  modular_server_.response().endObject();
}

void MixingValveController::startMixingHandler()
{
  ArduinoJson::JsonArray * ratio_ptr;
  modular_server_.parameter(constants::ratio_parameter_name).getValue(ratio_ptr);

  long valve_count;
  modular_server_.property(constants::valve_count_property_name).getValue(valve_count);

  ValveValues mix_ratio;
  for (size_t valve=0; valve<(size_t)valve_count; ++valve)
  {
    mix_ratio.push_back((*ratio_ptr)[valve]);
  }

  mix_ratio = startMixing(mix_ratio);

  modular_server_.response().returnResult(mix_ratio);
}

void MixingValveController::setMixDurationFactoredHandler()
{
  long duration;
  modular_server_.parameter(constants::duration_parameter_name).getValue(duration);

  long mix_duration = setMixDurationFactored(duration);
  modular_server_.response().returnResult(mix_duration);
}

void MixingValveController::stopMixingHandler(modular_server::Pin * pin_ptr)
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

    long valve_open_duration = mix_info_.valve_open_durations[mix_info_.valve];
    long valve_on_duration = 0;
    if (valve_open_duration > 0)
    {
      valve_on_duration = valve_open_duration + valve_switch_duration;
    }

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
