/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file    main_pico_weather_station.c
 * @author  Sebastian Lesse
 * @date    2022 / 07 / 25
 * @brief   Main file of the RPi pico weather station firmware
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include "string.h"

// --------------------------------------------------------------------------------

#include "initialization/initialization.h"
#include "mcu_task_management/mcu_task_controller.h"

// --------------------------------------------------------------------------------

#include "system_interface.h"
#include "cfg_driver_interface.h"
#include "time_management/time_management.h"
#include "modules/lcd/lcd_interface.h"
#include "signal_slot_interface.h"

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(MAIN_LCD_TIMER)

// --------------------------------------------------------------------------------

void task_yield(void) {
	mcu_task_controller_background_run();
}

// --------------------------------------------------------------------------------

/**
 * @brief After power-up we print a welcome screen
 * After this welcome screen has been printed on the LCD
 * we will configure th lcd-controller to not use any smooth update mode.
 */
static void main_LCD_UPDATED_SLOT_CALLBACK(void* p_argument) {
    (void) p_argument;

    static u8 configured = 0;

    if (configured == 0) {
        
        // we only want to configure the lcd-controller once.
        configured = 1;
        
        LCD_CONFIGUREATION lcd_cfg = {
            .refresh_mode = LCD_REFRESH_MODE_DIRECT,
            .refresh_pause = LCD_REFRESH_PAUSE_OFF
        };

        lcd_configure(&lcd_cfg);
    }
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    SIGNAL_LCD_UPDATED,
    MAIN_LCD_UPDATED_SLOT,
    main_LCD_UPDATED_SLOT_CALLBACK
)

// --------------------------------------------------------------------------------

void main_init(void) {

    ATOMIC_OPERATION
    (
        initialization();
    )

    DEBUG_PASS("main_init() - Initialization done");
}

int main( void ) {

    main_init();
    DEBUG_PASS(config_DEBUG_WELCOME_MESSAGE);

    MAIN_LCD_UPDATED_SLOT_connect();

    lcd_set_enabled(LCD_ENABLE);
    SIGNAL_LCD_LINE_send("                ");
    SIGNAL_LCD_LINE_send("   Welcome to   ");
    SIGNAL_LCD_LINE_send("  PICO Weather  ");
    SIGNAL_LCD_LINE_send("  Station  1.0  ");

    for (;;) {

        mcu_task_controller_schedule();
        task_yield();
        watchdog();
    }
}

// --------------------------------------------------------------------------------


/**
 * @brief We only need the strlen functionality of the common-tools-string module.
 * The common-tools-string module occupies more pogram memory then neded.
 * So, we implement this function here.
 * 
 * @see common_tools_string#common_tools_string_length
 */
u16 common_tools_string_length(const char* p_string) {
    return strlen(p_string);
}

// --------------------------------------------------------------------------------
