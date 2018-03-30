// ----------------------------------------------------------------------------
// MixingValveController.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef MIXING_VALVE_CONTROLLER_H
#define MIXING_VALVE_CONTROLLER_H
#include <ArduinoJson.h>
#include <JsonStream.h>
#include <Array.h>
#include <Vector.h>
#include <ConstantVariable.h>
#include <Functor.h>

#include <ModularClient.h>

#include <ModularServer.h>
#include <ModularDeviceBase.h>
#include <PowerSwitchController.h>

#include "MixingValveController/Constants.h"


class MixingValveController : public PowerSwitchController
{
public:
  MixingValveController();
  virtual void setup();

  typedef Array<long,mixing_valve_controller::constants::VALVE_COUNT_MAX> ValveValues;

  ValveValues startMixing(ValveValues ratio);
  void stopMixing();

  long getMixVolumeFillDuration();

  long setMixDurationFactored(const long duration);

protected:
  virtual bool finishMix();

private:
  modular_server::Property properties_[mixing_valve_controller::constants::PROPERTY_COUNT_MAX];
  modular_server::Parameter parameters_[mixing_valve_controller::constants::PARAMETER_COUNT_MAX];
  modular_server::Function functions_[mixing_valve_controller::constants::FUNCTION_COUNT_MAX];
  modular_server::Callback callbacks_[mixing_valve_controller::constants::CALLBACK_COUNT_MAX];

  mixing_valve_controller::constants::MixInfo mix_info_;

  // Mix Timing
  long mix_volume_fill_duration_;
  long mix_duration_min_;
  long mix_duration_max_;

  ValveValues normalizeRatio(ValveValues ratio);
  long valveValuesSum(ValveValues valve_values);
  size_t valveValuesMaxIndex(ValveValues valve_values);
  long constrainMixDuration(const long mix_duration,
                            const long mix_duration_min,
                            const long mix_duration_max);
  void setMixInfo(ValveValues ratio);

  // Handlers
  void setValveCountHandler();
  void updateMixPropertiesHandler();
  void getMixInfoHandler();
  void startMixingHandler();
  void setMixDurationFactoredHandler();
  void stopMixingHandler(modular_server::Interrupt * interrupt_ptr);
  void mixHandler(int arg);

};

#endif
