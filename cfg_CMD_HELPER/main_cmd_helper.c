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
 * @file   main_cmd_helper.c
 * @author Sebastian Lesse
 * @date   2021 / 12 / 27
 * @brief  SHC Command Helper main source file
 *          This program is used to generate a command for the shc-control-board
 *          by giving the name of the command.
 *
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include <stdio.h>

// --------------------------------------------------------------------------------

#include "initialization/initialization.h"
#include "common/signal_slot_interface.h"
#include "common/common_types.h"
#include "mcu_task_management/mcu_task_controller.h"
#include "ui/command_line/command_line_interface.h"
#include "ui/console/ui_console.h"
#include "ui/lcd/ui_lcd_interface.h"
#include "ui/cfg_file_parser/cfg_file_parser.h"

// --------------------------------------------------------------------------------

#include "ui/command_line/command_line_handler_remote_control.h"

// --------------------------------------------------------------------------------

/*!
 *
 */
static void main_RPI_HOST_TIMEOUT_SLOT_CALLBACK(const void* p_argument);

/*!
 *
 */
static void main_RPI_HOST_RESPONSE_RECEIVED_SLOT_CALLBACK(const void* p_argument);

/*!
 *
 */
static void main_RPI_HOST_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument);

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
static void main_CLI_LCD_ACTIVATED_SLOT_CALLBACK(const void* p_argument);

/*!
 *
 */
static void main_CFG_OBJECT_RECEIVED_SLOT_CALLBACK(const void* p_argument);

/*!
 *
 */
static void main_CLI_UNKNOWN_ARGUMENT_SLOT_CALLBACK(const void* p_argument);

/*!
 *
 */
static void main_CLI_NO_ARGUMENT_GIVEN_CALLBACK(const void* p_argument);

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_RESPONSE_TIMEOUT_SIGNAL, MAIN_RPI_HOST_TIMEOUT_SLOT, main_RPI_HOST_TIMEOUT_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_RESPONSE_RECEIVED_SIGNAL, MAIN_RPI_HOST_RESPONSE_RECEIVED_SLOT, main_RPI_HOST_RESPONSE_RECEIVED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_COMMAND_RECEIVED_SIGNAL, MAIN_RPI_HOST_COMMAND_RECEIVED_SLOT, main_RPI_HOST_COMMAND_RECEIVED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_HELP_REQUESTED_SIGNAL, MAIN_CLI_HELP_REQUESTED_SLOT, main_CLI_HELP_REQUESTED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_INVALID_PARAMETER_SIGNAL, MAIN_CLI_INVALID_PARAMETER_SLOT, main_CLI_INVALID_PARAMETER_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_LCD_ACTIVATED_SIGNAL, MAIN_CLI_LCD_ACTIVATED_SLOT, main_CLI_LCD_ACTIVATED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL, MAIN_CFG_OBJECT_RECEIVED_SLOT, main_CFG_OBJECT_RECEIVED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_UNKNOWN_ARGUMENT_SIGNAL, MAIN_CLI_UNKNOWN_ARGUMENT_SLOT, main_CLI_UNKNOWN_ARGUMENT_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_NO_ARGUMENT_GIVEN_SIGNAL, MAIN_CLI_NO_ARGUMENT_GIVEN_SLOT, main_CLI_NO_ARGUMENT_GIVEN_CALLBACK)

// --------------------------------------------------------------------------------

/**
 * @brief The program will be exit if this is set to 1.
 * E.g. calling the help page or command-response received.
 * 
 */
static u8 exit_program = 0;

// --------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

    ATOMIC_OPERATION
    (
        initialization();
    )

    DEBUG_PASS("main() - MAIN_RPI_HOST_RESPONSE_RECEIVED_SLOT_connect()");
    MAIN_RPI_HOST_RESPONSE_RECEIVED_SLOT_connect();

    DEBUG_PASS("main() - MAIN_RPI_HOST_COMMAND_RECEIVED_SLOT_connect()");
    MAIN_RPI_HOST_COMMAND_RECEIVED_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CLI_HELP_REQUESTED_SLOT_connect()");
    MAIN_CLI_HELP_REQUESTED_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CLI_INVALID_PARAMETER_SLOT_connect()");
    MAIN_CLI_INVALID_PARAMETER_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CLI_LCD_ACTIVATED_SLOT_connect()");
    MAIN_CLI_LCD_ACTIVATED_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CFG_OBJECT_RECEIVED_SLOT_connect()");
    MAIN_CFG_OBJECT_RECEIVED_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CLI_UNKNOWN_ARGUMENT_SLOT_connect()");
    MAIN_CLI_UNKNOWN_ARGUMENT_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CLI_NO_ARGUMENT_GIVEN_SLOT_connect()");
    MAIN_CLI_NO_ARGUMENT_GIVEN_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CLI_NO_ARGUMENT_GIVEN_SLOT_connect()");
    MAIN_RPI_HOST_TIMEOUT_SLOT_connect();

    command_line_interface(argc, argv);

    if (exit_program) {
        DEBUG_PASS("main() - PROGRAM EXIT REQUESTED !!! ---");
        return 1;
    }

    lcd_write_line("CMD-Helper");
    lcd_write_line("... started");

    for (;;) {

        if (exit_program) {
            break;
        }
        
        mcu_task_controller_schedule();
        mcu_task_controller_background_run();
        watchdog();
    }

    return 0;
}

// --------------------------------------------------------------------------------

static void main_RPI_HOST_RESPONSE_RECEIVED_SLOT_CALLBACK(const void* p_argument) {

    if (p_argument == NULL) {
        DEBUG_PASS("main_RPI_HOST_RESPONSE_RECEIVED_SLOT_CALLBACK() - argument is NULL");
        return;
    }

    DEBUG_PASS("main_RPI_HOST_RESPONSE_RECEIVED_SLOT_CALLBACK()");

    COMMON_GENERIC_BUFFER_TYPE* p_buffer = (COMMON_GENERIC_BUFFER_TYPE*) p_argument;

    u8 t_buffer[128];
    t_buffer[0] = p_buffer->length;

    if (p_buffer->length > sizeof(t_buffer) - 1) {
        p_buffer->length = sizeof(t_buffer) - 1;
    }

    memcpy(t_buffer + 1, p_buffer->data, p_buffer->length);

    console_write_line("Response:");
    console_hex_dump(p_buffer->length + 1, t_buffer);

    lcd_write_line("CMD-Helper");
    lcd_write_line("- response OK");

    exit_program = 1;
}

static void main_RPI_HOST_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument) {

    DEBUG_PASS("main_RPI_HOST_COMMAND_RECEIVED_SLOT_CALLBACK()");

    COMMON_GENERIC_BUFFER_TYPE* p_buffer = (COMMON_GENERIC_BUFFER_TYPE*) p_argument;

    console_write_line("Command:");
    console_hex_dump(p_buffer->length, p_buffer->data);

    lcd_write_line("CMD-Helper");
    lcd_write_line("- command OK");
}

static void main_RPI_HOST_TIMEOUT_SLOT_CALLBACK(const void* p_argument) {

    (void) p_argument;
    DEBUG_PASS("main_RPI_HOST_TIMEOUT_SLOT_CALLBACK()");

    exit_program = 1;

    console_write_line("TIMEOUT !");

    lcd_write_line("CMD-Helper");
    lcd_write_line("- TIMEOUT !");
}

// --------------------------------------------------------------------------------

static void main_CLI_INVALID_PARAMETER_SLOT_CALLBACK(const void* p_argument) {

    DEBUG_PASS("main_CLI_INVALID_PARAMETER_SLOT_CALLBACK");

    if (p_argument != NULL) {
        printf("Invalid parameter for arguemnt %s given!\n", (char*)p_argument);
    } else {
        console_write_line("Invalid parameter given, check your input!");
    }
    
    lcd_write_line("CMD-Helper");
    lcd_write_line("- inv parameter");

    main_CLI_HELP_REQUESTED_SLOT_CALLBACK(NULL);
}

static void main_CLI_LCD_ACTIVATED_SLOT_CALLBACK(const void* p_argument) {

    DEBUG_PASS("main_CLI_LCD_ACTIVATED_SLOT_CALLBACK()");

    lcd_init();
    lcd_set_enabled(1);
}

static void main_CLI_HELP_REQUESTED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;

    console_write("SHC CMD-Helper Version: ");
    console_write_number(VERSION_MAJOR);
    console_write(".");
    console_write_number(VERSION_MINOR);

    console_new_line();
    console_new_line();

    console_write_line("Usage: cmdHelper -remote <REMOTE-COMMAND>");
    console_new_line();
    console_write_line("Available <REMOTE-COMMAND>'s:");
    console_new_line();
    console_write_line("    JVC device commands:");

    CLI_REMOTE_CONTROL_COMMAND_PAIR jvc_commands[] = { CLI_REMOTE_CONTROL_CMD_ARRAY_JVC };
    u16 i = 0;

    for ( ; i < CLI_REMOTE_SIZEOF_COMMAND_PAIR_TABLE(jvc_commands) ; i++ ) {
        console_new_line();
        console_write_string("      ", jvc_commands[i].cmd_name);
    }

    console_new_line();
    console_write_line("    Samsung device commands:");

    CLI_REMOTE_CONTROL_COMMAND_PAIR samsung_commands[] = { CLI_REMOTE_CONTROL_CMD_ARRAY_SAMSUNG };
    i = 0;

    for ( ; i < CLI_REMOTE_SIZEOF_COMMAND_PAIR_TABLE(samsung_commands) ; i++ ) {
        console_new_line();
        console_write_string("      ", samsung_commands[i].cmd_name);
    }

    console_new_line();
    console_write_line("    Sony device commands:");

    CLI_REMOTE_CONTROL_COMMAND_PAIR sony_commands[] = { CLI_REMOTE_CONTROL_CMD_ARRAY_SONY_BDPLAYER };
    i = 0;

    for ( ; i < CLI_REMOTE_SIZEOF_COMMAND_PAIR_TABLE(sony_commands) ; i++ ) {
        console_new_line();
        console_write_string("      ", sony_commands[i].cmd_name);
    }

    console_new_line();
    console_write_line("    LED-Lights device commands:");

    CLI_REMOTE_CONTROL_COMMAND_PAIR led_lights_commands[] = { CLI_REMOTE_CONTROL_CMD_ARRAY_LED_LIGHTS };
    i = 0;

    for ( ; i < CLI_REMOTE_SIZEOF_COMMAND_PAIR_TABLE(led_lights_commands) ; i++ ) {
        console_new_line();
        console_write_string("      ", led_lights_commands[i].cmd_name);
    }

    exit_program = 1;
}

static void main_CFG_OBJECT_RECEIVED_SLOT_CALLBACK(const void* p_argument) {

    CFG_FILE_PARSER_CFG_OBJECT_TYPE* p_cfg_obj = (CFG_FILE_PARSER_CFG_OBJECT_TYPE*)p_argument;

    console_write_line("CONFIGURATIATION-OBJECT:");
    console_write_string("- key: ", p_cfg_obj->key);
    console_write_string("- value: ", p_cfg_obj->value);
}

static void main_CLI_UNKNOWN_ARGUMENT_SLOT_CALLBACK(const void* p_argument) {

    DEBUG_PASS("main_CLI_UNKNOWN_ARGUMENT_SLOT_CALLBACK()");
    
    lcd_write_line("CMD-Helper");
    lcd_write_line("- inv parameter");

    if (p_argument == NULL) {

        DEBUG_PASS("main_CLI_UNKNOWN_ARGUMENT_SLOT_CALLBACK() - NULLPOINTER EXCEPTION");
        console_write_line("Unknown argument given given!");

    } else {

        COMMAND_LINE_ARGUMENT_TYPE* p_unknown_argument = (COMMAND_LINE_ARGUMENT_TYPE*) p_argument;
        console_write_string("Unknown argument given ", p_unknown_argument->argument);
    }

    exit_program = 1;
}

static void main_CLI_NO_ARGUMENT_GIVEN_CALLBACK(const void* p_argument) {

    DEBUG_PASS("main_CLI_NO_ARGUMENT_GIVEN_CALLBACK()");
    console_write_line("No argument was given !");
    console_write_line("give -help to see a list of valid arguments");

    exit_program = 1;
}
