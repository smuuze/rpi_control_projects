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
 * @file   main_file_hexdump.c
 * @author Sebastian Lesse
 * @date   2021 / 05 / 26
 * @brief  
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
#include <fcntl.h>

// --------------------------------------------------------------------------------

#include "initialization/initialization.h"
#include "common/signal_slot_interface.h"
#include "common/common_types.h"

#include "mcu_task_management/mcu_task_controller.h"

#include "ui/command_line/command_line_interface.h"
#include "ui/console/ui_console.h"

#include "common/common_tools_string.h"

// --------------------------------------------------------------------------------

#define MAIN_MODULE_NAME                "FILE HEXDUMP"
#define MAIN_MAX_LENGTH_FILE_PATH       1024
#define MAIN_READ_BUFFER_MAX_LEN        4096

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
static void main_CLI_ARGUMENT_FILE_SLOT_CALLBACK(const void* p_argument);

/*!
 *
 */
static void main_CLI_ARGUMENT_N_SLOT_CALLBACK(const void* p_argument);

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_HELP_REQUESTED_SIGNAL, MAIN_CLI_HELP_REQUESTED_SLOT, main_CLI_HELP_REQUESTED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_INVALID_PARAMETER_SIGNAL, MAIN_CLI_INVALID_PARAMETER_SLOT, main_CLI_INVALID_PARAMETER_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_ARGUMENT_FILE_SIGNAL, CLI_ARGUMENT_FILE_SLOT, main_CLI_ARGUMENT_FILE_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_ARGUMENT_N_SIGNAL, CLI_ARGUMENT_N_SLOT, main_CLI_ARGUMENT_N_SLOT_CALLBACK)



// --------------------------------------------------------------------------------

/**
 * @brief local variable that holds the cli-command data given by the user
 * Is reseted at program-start and is set via CLI_ARGUMENT_GPIO_SIGNAL
 * 
 */
static char file_path[MAIN_MAX_LENGTH_FILE_PATH];

/**
 * @brief number of characters / bytes to read from the given file
 * 
 */
static u32 char_count = 32;

// --------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

    ATOMIC_OPERATION
    (
        initialization();
    )

    common_tools_string_clear(file_path, MAIN_MAX_LENGTH_FILE_PATH);

    DEBUG_PASS("main() - MAIN_CLI_HELP_REQUESTED_SLOT_connect()");
    MAIN_CLI_HELP_REQUESTED_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CLI_INVALID_PARAMETER_SLOT_connect()");
    MAIN_CLI_INVALID_PARAMETER_SLOT_connect();

    DEBUG_PASS("main() - CLI_ARGUMENT_FILE_SLOT_connect()");
    CLI_ARGUMENT_FILE_SLOT_connect();

    DEBUG_PASS("main() - CLI_ARGUMENT_N_SLOT_connect()");
    CLI_ARGUMENT_N_SLOT_connect();

    command_line_interface(argc, argv);

    int handle = open(file_path, O_RDONLY);
    if (handle < 0) {
        console_write_line("OPEN FILE HAS FAILED!");
        return -1;
    }

    u8 buffer[MAIN_READ_BUFFER_MAX_LEN];

    if (read(handle, &buffer[0], char_count) <= 0 ) {
        console_write_line("READING FILE HAS FAILED!");
        close(handle);
        return -1;
    }

    common_tools_hex_dump(&buffer[0], char_count);
    
    close(handle);

    mcu_task_controller_terminate_all();

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

    console_write("SHC FILE-HEXDUMP Version: ");
    console_write_number(VERSION_MAJOR);
    console_write(".");
    console_write_number(VERSION_MINOR);

    console_new_line();
    console_new_line();

    console_write_line("Usage: fileHexdump <OPTIONS>\n\n");
    console_write_line("OPTIONS:");

    console_write_line("-file <FILE-PATH>           : path to the file to read from");
    console_write_line("-n                          : number of bytes / characters to read from the file (default is 32)");
    console_write_line("-h                          : Print this help screen");
}

// --------------------------------------------------------------------------------

static void main_CLI_ARGUMENT_FILE_SLOT_CALLBACK(const void* p_argument) {
    common_tools_string_append(file_path, (const char*)p_argument, MAIN_MAX_LENGTH_FILE_PATH); 
}

// --------------------------------------------------------------------------------

static void main_CLI_ARGUMENT_N_SLOT_CALLBACK(const void* p_argument) {
    const u32* p_char_count = (const u32*)p_argument;
    char_count = *p_char_count;

    if (char_count > MAIN_READ_BUFFER_MAX_LEN) {
        char_count = MAIN_READ_BUFFER_MAX_LEN;
    }
}

// --------------------------------------------------------------------------------
