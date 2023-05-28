#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

// --------------------------------------------------------------------------------

#define BOARD_DESCRIPTION_FILE			"platine/board_PI_PICO.h"
#include "platine/board_PI_PICO.h"

// --------------------------------------------------------------------------------

#define config_DEBUG_WELCOME_MESSAGE    "Welcome to RPi-Pico Weater-Station"

// --------------------------------------------------------------------------------

#define _config_PROGRAM_DESCRIPTION     "Implementation of the Chipdesign "         \
                                        "Weather-Station useable on a RP2040 MCU"

#define _config_PROGRAM_NAME            "Pico-Weather-Station"

#define _config_PROGRAM_URL             "https://github.com/smuuze/rpi_control_projects/cfg_PICO_WEATHER_STATION"

// --------------------------------------------------------------------------------

#define MOVEMENT_DETECTION_CONTROLLER_WAIT_TO_VERIFY_TIMEOUT_MS         100

// --------------------------------------------------------------------------------

#endif /* _config_H_ */

// --------------------------------------------------------------------------------
