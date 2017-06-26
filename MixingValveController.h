// ----------------------------------------------------------------------------
// MixingValveController.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef MIXING_VALVE_CONTROLLER_H
#define MIXING_VALVE_CONTROLLER_H
#include "Flash.h"
#include <EEPROM.h>
#include "Streaming.h"
#include "ArduinoJson.h"
#include "JsonSanitizer.h"
#include "JsonStream.h"
#include "Array.h"
#include "Vector.h"
#include "MemoryFree.h"
#include "ConstantVariable.h"
#include "SavedVariable.h"
#include "Functor.h"
#include "IndexedContainer.h"
#include "FunctorCallbacks.h"

#include "EventController.h"

#include "ModularServer.h"
#include "ModularDeviceBase.h"
#include "PowerSwitchController.h"

#include "utility/Constants.h"


class MixingValveController : public PowerSwitchController
{
public:
  MixingValveController();
  virtual void setup();

private:
  modular_server::Property properties_[mixing_valve_controller::constants::PROPERTY_COUNT_MAX];
  modular_server::Parameter parameters_[mixing_valve_controller::constants::PARAMETER_COUNT_MAX];
  modular_server::Function functions_[mixing_valve_controller::constants::FUNCTION_COUNT_MAX];
  modular_server::Callback callbacks_[mixing_valve_controller::constants::CALLBACK_COUNT_MAX];

  // Handlers

};

#endif
