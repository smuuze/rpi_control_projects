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
#include "ui/lcd/lcd_interface.h"

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(MAIN_LED_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U16(MAIN_LCD_TIMER)

INCLUDE_GPIO(GPIO_25)

// --------------------------------------------------------------------------------

void task_yield(void) {
	mcu_task_controller_background_run();
}

// --------------------------------------------------------------------------------

void main_init(void) {

    ATOMIC_OPERATION
    (
        initialization();
    )

    #ifdef HAS_GPIO_LED_RED
    {
        LED_RED_drive_low();
    }
    #endif

    DEBUG_PASS("main_init() - Initialization done");
}

int main( void ) {

    main_init();
    DEBUG_PASS(config_DEBUG_WELCOME_MESSAGE);

    u8 i = 0u;

    lcd_set_enabled(LCD_ENABLE);

    MAIN_LED_TIMER_start();
    GPIO_25_drive_high();

    MAIN_LCD_TIMER_start();
    u8 lcd_line_index = 0;


    static const char lcd_line_array[4][17] = {
        "LCD LINE ONE    \0",
        "LCD LINE TWO    \0",
        "LCD LINE THREE  \0",
        "LCD LINE FOUR   \0"
    };

    for (;;) {

        if (MAIN_LED_TIMER_is_up(500)) {
            MAIN_LED_TIMER_start();
            GPIO_25_toggle_level();

            DEBUG_TRACE_byte(i, "Value of i is");
            i += 1u;
        }

        if (MAIN_LCD_TIMER_is_up(5000)) {
            MAIN_LCD_TIMER_start();
            SIGNAL_LCD_LINE_send(lcd_line_array[lcd_line_index]);

            if (++lcd_line_index >= 4) {
                lcd_line_index = 0;
            }
        }

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
