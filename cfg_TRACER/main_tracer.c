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
 * @file   main_tracer.c
 * @author Sebastian Lesse
 * @date   2021 / 12 / 27
 * @brief  Short description of this file
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
#include "common/qeue_interface.h"
#include "common/common_tools_string.h"

#include "mcu_task_management/mcu_task_controller.h"
#include "mcu_task_management/thread_interface.h"

#include "ui/command_line/command_line_interface.h"
#include "ui/console/ui_console.h"
#include "ui/lcd/ui_lcd_interface.h"
#include "modules/cfg_file_parser/cfg_file_parser.h"

#include "tracer/trace_object.h"
#include "app_tasks/thread_read_trace_object.h"
#include "app_tasks/thread_parse_trace_object.h"
#include "app_tasks/thread_print_trace_object.h"

#include "driver/trx_driver_interface.h"
#include "driver/cfg_driver_interface.h"

#include "system/system_interface.h"

// --------------------------------------------------------------------------------

#ifndef TRACER_DEFAULT_COM_DRIVER_DEVICE
#define TRACER_DEFAULT_COM_DRIVER_DEVICE            "/dev/serial0"
#endif

// --------------------------------------------------------------------------------

#ifndef TRACER_RAW_TRACE_OBJECT_QEUE_SIZE
#define TRACER_RAW_TRACE_OBJECT_QEUE_SIZE           100
#endif

#ifndef TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE
#define TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE        100
#endif

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void main_CLI_HELP_REQUESTED_SLOT_CALLBACK(const void* p_argument);

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void main_CLI_INVALID_PARAMETER_SLOT_CALLBACK(const void* p_argument);

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void main_CLI_UNKNOWN_ARGUMENT_SLOT_CALLBACK(const void* p_argument);

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void main_CLI_NO_ARGUMENT_GIVEN_CALLBACK(const void* p_argument);

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void main_CLI_ARGUMENT_DEVICE_SIGNAL_CALLBACK(const void* p_argument);

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_HELP_REQUESTED_SIGNAL, MAIN_CLI_HELP_REQUESTED_SLOT, main_CLI_HELP_REQUESTED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_INVALID_PARAMETER_SIGNAL, MAIN_CLI_INVALID_PARAMETER_SLOT, main_CLI_INVALID_PARAMETER_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_UNKNOWN_ARGUMENT_SIGNAL, MAIN_CLI_UNKNOWN_ARGUMENT_SLOT, main_CLI_UNKNOWN_ARGUMENT_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_NO_ARGUMENT_GIVEN_SIGNAL, MAIN_CLI_NO_ARGUMENT_GIVEN_SLOT, main_CLI_NO_ARGUMENT_GIVEN_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_ARGUMENT_DEVICE_SIGNAL, MAIN_CLI_ARGUMENT_DEVICE_SIGNAL_SLOT, main_CLI_ARGUMENT_DEVICE_SIGNAL_CALLBACK)

// --------------------------------------------------------------------------------

QEUE_INTERFACE_BUILD_QEUE(RAW_TRACE_OBJECT_QEUE, TRACE_OBJECT_RAW, sizeof(TRACE_OBJECT_RAW), TRACER_RAW_TRACE_OBJECT_QEUE_SIZE)
QEUE_INTERFACE_BUILD_QEUE(TRACE_OBJECT_QEUE, TRACE_OBJECT, sizeof(TRACE_OBJECT), TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE)

// --------------------------------------------------------------------------------

/*!
 *
 */
static u8 exit_program = 0;

/**
 * @brief holds the configuration of the communication driver
 * that is used to read trace data. By default a serial communicaiton
 * is used.
 * 
 */
static TRX_DRIVER_CONFIGURATION driver_cfg;

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char* argv[]) {

    ATOMIC_OPERATION
    (
        initialization();

        READ_TRACE_OBJECT_THREAD_init();
        PARSE_TRACE_OBJECT_THREAD_init();
        PRINT_TRACE_OBJECT_THREAD_init();
    )

    {
        DEBUG_PASS("main() - Initialize Slots");

        DEBUG_PASS("main() - MAIN_CLI_HELP_REQUESTED_SLOT_connect()");
        MAIN_CLI_HELP_REQUESTED_SLOT_connect();

        DEBUG_PASS("main() - MAIN_CLI_INVALID_PARAMETER_SLOT_connect()");
        MAIN_CLI_INVALID_PARAMETER_SLOT_connect();

        DEBUG_PASS("main() - MAIN_CLI_UNKNOWN_ARGUMENT_SLOT_connect()");
        MAIN_CLI_UNKNOWN_ARGUMENT_SLOT_connect();

        DEBUG_PASS("main() - MAIN_CLI_NO_ARGUMENT_GIVEN_SLOT_connect()");
        MAIN_CLI_NO_ARGUMENT_GIVEN_SLOT_connect();

        DEBUG_PASS("main() - MAIN_CLI_ARGUMENT_DEVICE_SIGNAL_SLOT_connect()");
        MAIN_CLI_ARGUMENT_DEVICE_SIGNAL_SLOT_connect();
    }

    {
        DEBUG_PASS("main() - Initialize communication driver");

        driver_cfg.module.usart.baudrate = BAUDRATE_230400;
        driver_cfg.module.usart.databits = DATABITS_8;
        driver_cfg.module.usart.parity = PARITY_NONE;
        driver_cfg.module.usart.stopbits = STOPBITS_1;

        common_tools_string_clear(driver_cfg.device.name, DRIVER_CFG_DEVICE_NAME_MAX_LENGTH);
        common_tools_string_copy_string(driver_cfg.device.name, TRACER_DEFAULT_COM_DRIVER_DEVICE, DRIVER_CFG_DEVICE_NAME_MAX_LENGTH);
    }

    command_line_interface(argc, argv);

    if (exit_program) {
        DEBUG_PASS("main() - PROGRAM EXIT REQUESTED !!! ---");
        return 1;
    }
    
    RAW_TRACE_OBJECT_QEUE_init();
    TRACE_OBJECT_QEUE_init();


    i_system.driver.usart0->initialize();
    i_system.driver.usart0->configure(&driver_cfg);
    i_system.driver.usart0->start_rx(TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH);

    thread_read_trace_object_set_com_driver(i_system.driver.usart0);

    READ_TRACE_OBJECT_THREAD_start();
    PARSE_TRACE_OBJECT_THREAD_start();
    PRINT_TRACE_OBJECT_THREAD_start();

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

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void main_CLI_INVALID_PARAMETER_SLOT_CALLBACK(const void* p_argument) {

    DEBUG_PASS("main_CLI_INVALID_PARAMETER_SLOT_CALLBACK");

    if (p_argument != NULL) {
        printf("Invalid parameter for arguemnt %s given!\n", (char*)p_argument);
    } else {
        console_write_line("Invalid parameter given, check your input!");
    }

    main_CLI_HELP_REQUESTED_SLOT_CALLBACK(NULL);
}

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void main_CLI_HELP_REQUESTED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;

    console_write("SHC Tracer Version: ");
    console_write_number(VERSION_MAJOR);
    console_write(".");
    console_write_number(VERSION_MINOR);

    console_new_line();
    console_new_line();

    console_write_line("Usage: shcTracer [options]]");
    console_write_line("Options:");
    console_write_line("-dev <device_file>                 : device to use for reading trace data");
    console_write_line("-path <path>                       : path to directory that includes your makefile");
    console_write_line("-file <path>                       : traceoutput will be stored into this file");
    console_write_line("-console                           : traceoutput will be shown on console");
    console_write_line("-mqtt <topic>@<servicer_ip:port>   : traceoutput will be shown on console");

    exit_program = 1;
}

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void main_CLI_UNKNOWN_ARGUMENT_SLOT_CALLBACK(const void* p_argument) {

    DEBUG_PASS("main_CLI_UNKNOWN_ARGUMENT_SLOT_CALLBACK()");

    if (p_argument == NULL) {

        DEBUG_PASS("main_CLI_UNKNOWN_ARGUMENT_SLOT_CALLBACK() - NULLPOINTER EXCEPTION");
        console_write_line("Unknown argument given given!");

    } else {

        COMMAND_LINE_ARGUMENT_TYPE* p_unknown_argument = (COMMAND_LINE_ARGUMENT_TYPE*) p_argument;
        console_write_string("Unknown argument given ", p_unknown_argument->argument);
    }

    exit_program = 1;
}

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void main_CLI_NO_ARGUMENT_GIVEN_CALLBACK(const void* p_argument) {

    DEBUG_PASS("main_CLI_NO_ARGUMENT_GIVEN_CALLBACK()");
    console_write_line("No argument was given !");
    console_write_line("give -help to see a list of valid arguments");

    exit_program = 1;
}

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void main_CLI_ARGUMENT_DEVICE_SIGNAL_CALLBACK(const void* p_argument) {

    if (p_argument == NULL) {
        DEBUG_PASS("main_CLI_ARGUMENT_DEVICE_SIGNAL_CALLBACK() - NULL_POINTER_EXCEPTION");
        return;
    }

    const char* p_string = (const char*)p_argument;
    DEBUG_TRACE_STR(p_string, "main_CLI_ARGUMENT_DEVICE_SIGNAL_CALLBACK() - Device");

    common_tools_string_clear(driver_cfg.device.name, DRIVER_CFG_DEVICE_NAME_MAX_LENGTH);
    common_tools_string_copy_string(driver_cfg.device.name, p_string, DRIVER_CFG_DEVICE_NAME_MAX_LENGTH);
}
