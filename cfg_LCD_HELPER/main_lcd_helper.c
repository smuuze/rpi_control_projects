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
 * @file   main_lcd_reader.c
 * @author Sebastian Lesse
 * @date   2021 / 06 / 05
 * @brief  
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

#include "initialization/initialization.h"
#include "common/signal_slot_interface.h"
#include "common/common_types.h"

#include "mcu_task_management/mcu_task_controller.h"

#include "ui/command_line/command_line_interface.h"
#include "ui/command_line/command_line_handler_gpio.h"
#include "ui/console/ui_console.h"

#include "common/common_tools_string.h"
#include "ui/lcd/ui_lcd_interface.h"

// --------------------------------------------------------------------------------

#define MAIN_MODULE_NAME            "LCD Helper"

// --------------------------------------------------------------------------------

/*!
 *
 */
static void main_CLI_HELP_REQUESTED_SLOT_CALLBACK(const void* p_argument);

/*!
 *
 */
static void main_CLI_INVALID_PARAMETER_SLOT_CALLBACK(const void* p_argument);

/*!
 *
 */
static void main_CCLI_LCD_ACTIVATED_SLOT_CALLBACK(const void* p_argument);

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_HELP_REQUESTED_SIGNAL, MAIN_CLI_HELP_REQUESTED_SLOT, main_CLI_HELP_REQUESTED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_INVALID_PARAMETER_SIGNAL, MAIN_CLI_INVALID_PARAMETER_SLOT, main_CLI_INVALID_PARAMETER_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_LCD_ACTIVATED_SIGNAL, CLI_LCD_ACTIVATED_SLOT, main_CCLI_LCD_ACTIVATED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

/**
 * @brief Buffer to remember the string that
 * was given by the user via command-line.
 * 
 */
static char lcd_string[33];

/**
 * @brief Number of charactesr that are copied into lcd_string.
 * If this variable is still 0 after command_line_interface()
 * A error-message is printed.
 * 
 */
static u16 lcd_string_length = 0;

// --------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

    ATOMIC_OPERATION
    (
        initialization();
    )

    common_tools_string_clear(lcd_string, sizeof(lcd_string));

    DEBUG_PASS("main() - MAIN_CLI_HELP_REQUESTED_SLOT_connect()");
    MAIN_CLI_HELP_REQUESTED_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CLI_INVALID_PARAMETER_SLOT_connect()");
    MAIN_CLI_INVALID_PARAMETER_SLOT_connect();

    DEBUG_PASS("main() - CLI_LCD_ACTIVATED_SLOT_connect()");
    CLI_LCD_ACTIVATED_SLOT_connect();

    command_line_interface(argc, argv);

    if (lcd_string_length != 0) {
        lcd_write_line(lcd_string);

    } else {
        main_CLI_INVALID_PARAMETER_SLOT_CALLBACK(NULL);
    }

    mcu_task_controller_terminate_all();

    ATOMIC_OPERATION
    (
        deinitialization();
    )

    return 0;
}

// --------------------------------------------------------------------------------

static void main_CLI_INVALID_PARAMETER_SLOT_CALLBACK(const void* p_argument) {

    DEBUG_PASS("main_CLI_INVALID_PARAMETER_SLOT_CALLBACK");

    if (p_argument != NULL) {
        printf("Invalid parameter for arguemnt %s given!\n", (char*)p_argument);
    } else {
        console_write_line("Invalid parameter given, check your input!");
    }

    main_CLI_HELP_REQUESTED_SLOT_CALLBACK(NULL);
}

// --------------------------------------------------------------------------------

static void main_CLI_HELP_REQUESTED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;

    console_write("SHC " MAIN_MODULE_NAME " Version: ");
    console_write_number(VERSION_MAJOR);
    console_write(".");
    console_write_number(VERSION_MINOR);

    console_new_line();
    console_new_line();

    console_write_line("Usage: gpioHelper <OPTIONS>\n\n");
    console_write_line("OPTIONS:");

    console_write_line("-lcd <STRING>                 Prints the given <STRING> on the actual connected LCD");
    console_write_line("                              The String is autoamtically splitted to fit the into");
    console_write_line("                              the lines of the LCD-string");
    console_new_line();
    console_write_line("-h                            Print this help screen");
}

// --------------------------------------------------------------------------------

static void main_CCLI_LCD_ACTIVATED_SLOT_CALLBACK(const void* p_argument) {

    if (p_argument == NULL) {
        console_write_line("FATAL: NULL-POINTER-EXCEPTION");
        main_CLI_HELP_REQUESTED_SLOT_CALLBACK(NULL);
        return;
    }

    lcd_set_enabled(LCD_ENABLE);
    lcd_init();

    const char* p_string = (const char*) p_argument;
    lcd_string_length = common_tools_string_length(p_string);

    if (lcd_string_length > sizeof(lcd_string)) {
        lcd_string_length = sizeof(lcd_string) - 1; // -1 to respect 0-termination
    }

    DEBUG_TRACE_STR(p_string, "main_CCLI_LCD_ACTIVATED_SLOT_CALLBACK() - String:");
    common_tools_string_copy_string(lcd_string, p_string, sizeof(lcd_string));
}

// --------------------------------------------------------------------------------
