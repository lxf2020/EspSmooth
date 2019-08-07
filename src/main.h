#pragma once

// work as a mqtt sensor.
int16_t current_working_state;
int16_t actuators_position[6];


//defined in esphome.  be invoked out of esphome.
#include "esphome/components/logger/logger.h"
extern esphome::logger::Logger *logger_logger;
extern void esphome_setup();

