#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

// --------------------------------------------------------------------------------

#define BOARD_DESCRIPTION_FILE			"platine/board_RASPBERRYPI.h"
#include "platine/board_RASPBERRYPI.h"

// --------------------------------------------------------------------------------

#define config_DEBUG_WELCOME_MESSAGE    "Welcome to Raspberry Pi Weater-Station"

// --------------------------------------------------------------------------------

#define _config_PROGRAM_DESCRIPTION     "Implementation of the Chipdesign "         \
                                        "Weather-Station useable on a Raspberry Pi"

#define _config_PROGRAM_NAME            "RPi-Weather-Station"

#define _config_PROGRAM_URL             "https://github.com/smuuze/rpi_control_projects/cfg_RPI_WEATHER_STATION"

// --------------------------------------------------------------------------------

#define MOVEMENT_DETECTION_CONTROLLER_WAIT_TO_VERIFY_TIMEOUT_MS         100

// --------------------------------------------------------------------------------

#endif /* _config_H_ */

// --------------------------------------------------------------------------------
