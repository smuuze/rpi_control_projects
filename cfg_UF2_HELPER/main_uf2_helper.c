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
 * @file    main_uf2_helper.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 31
 * @brief   Can handle UF2 files. Like reading and showing blocks
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

#include <stdio.h>

// --------------------------------------------------------------------------------

#include "initialization/initialization.h"
#include "common/signal_slot_interface.h"
#include "common/common_types.h"
#include "mcu_task_management/mcu_task_controller.h"
#include "ui/command_line/command_line_interface.h"
#include "ui/console/ui_console.h"
#include "ui/file_interface/file_interface.h"
#include "protocol_management/uf2/uf2_interface.h"

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

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_ARGUMENT_FILE_SIGNAL, MAIN_CLI_ARGUMENT_FILE_SLOT, main_CLI_ARGUMENT_FILE_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_ARGUMENT_N_SIGNAL, MAIN_CLI_ARGUMENT_N_SLOT, main_CLI_ARGUMENT_N_SLOT_CALLBACK)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_HELP_REQUESTED_SIGNAL, MAIN_CLI_HELP_REQUESTED_SLOT, main_CLI_HELP_REQUESTED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_INVALID_PARAMETER_SIGNAL, MAIN_CLI_INVALID_PARAMETER_SLOT, main_CLI_INVALID_PARAMETER_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

FILE_INTERFACE_CREATE_FILE(UF2_FILE)

// --------------------------------------------------------------------------------

UF2_CREATE_BLOCK(MAIN_UF2_BLOCK)

// --------------------------------------------------------------------------------

/**
 * @brief If is set to 1 the program will be exited.
 * 
 */
static u8 exit_program = 0;

/**
 * @brief Is set to 1 if the file-path was set via the command line interface
 * 
 */
static u8 file_set = 0;

/**
 * @brief Optional parameter holding the max number of blocks to print
 * 
 */
static u32 block_count = 0xFFFFFFFF;

// --------------------------------------------------------------------------------

/**
 * @brief Reads and parses the file that was given
 * by the -file command line argument.
 * 
 */
static void parse_uf2_file(void) {

    if (UF2_FILE_is_existing() == 0) {
        console_write_line("FILE NOT FOUND!");
        return;
    }

    if (UF2_FILE_is_readable() == 0) {
        console_write_line("FILE CANNOT BE READ!");
        return;
    }

    if (UF2_FILE_open() == 0) {
        console_write_line("OPEN FILE FAILED!");
        return;
    }

    u8 byte_array[UF2_BLOCK_NUM_BYTES];
    memset(byte_array, 0x00, sizeof(byte_array));

    u16 num_bytes_read = 0;
    u16 offset = 0;

    while ((num_bytes_read = UF2_FILE_read_bytes(byte_array, offset, UF2_BLOCK_NUM_BYTES)) != 0) {

        if (num_bytes_read != UF2_BLOCK_NUM_BYTES) {
            console_write_line("INVALID BLOCK-SIZE");
            return;
        }

        if (MAIN_UF2_BLOCK_parse(byte_array) != UF2_ERROR_NONE) {
            console_write_line("PARSE UF2-BLOCK HAS FAILED");
            return;
        }

        printf("%s\n", "---------------------------------------------------------");
        printf("%s%u\n", "BLOCK-NUMBER    : ", MAIN_UF2_BLOCK_get_block_number());
        printf("%s%u\n", "MAX-BLOCK-NUMBER: ", MAIN_UF2_BLOCK_get_number_of_blocks());
        printf("%s%u (0x%08X)\n", "FLAGS           : ", MAIN_UF2_BLOCK_get_flags(), MAIN_UF2_BLOCK_get_flags());
        printf("%s%u (0x%08X)\n", "BOARD-FAMILY    : ", MAIN_UF2_BLOCK_get_file_size(), MAIN_UF2_BLOCK_get_file_size());
        printf("%s%u (0x%08X)\n", "TARGET-ADDRESS  : ", MAIN_UF2_BLOCK_get_target_address(), MAIN_UF2_BLOCK_get_target_address());
        printf("%s%u (0x%08X)\n", "PAYLOAD-SIZE    : ", MAIN_UF2_BLOCK_get_payload_size(), MAIN_UF2_BLOCK_get_payload_size());

        console_hex_dump(MAIN_UF2_BLOCK_get_payload_size(), MAIN_UF2_BLOCK_get_payload());

        offset += UF2_BLOCK_NUM_BYTES;

        if (block_count != 0xFFFFFFFF) {
            if (--block_count == 0) {
                break;
            }
        }
    }

    UF2_FILE_close();
}

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
    )

    DEBUG_PASS("main() - MAIN_CLI_HELP_REQUESTED_SLOT_connect()");
    MAIN_CLI_HELP_REQUESTED_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CLI_INVALID_PARAMETER_SLOT_connect()");
    MAIN_CLI_INVALID_PARAMETER_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CLI_ARGUMENT_FILE_SLOT_connect()");
    MAIN_CLI_ARGUMENT_FILE_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CLI_ARGUMENT_N_SLOT_connect()");
    MAIN_CLI_ARGUMENT_N_SLOT_connect();

    command_line_interface(argc, argv);

    if (exit_program) {
        DEBUG_PASS("main() - initialization FAILED !!! --- ");
        return 1;
    }

    for (;;) {

        if (file_set == 1) {
            parse_uf2_file();
            break;
        }

        if (exit_program) {
            break;
        }
        
        mcu_task_controller_schedule();
        mcu_task_controller_background_run();
        watchdog();
    }

    ATOMIC_OPERATION
    (
        deinitialization();
    )

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
static void main_CLI_ARGUMENT_FILE_SLOT_CALLBACK(const void* p_argument) {

    DEBUG_PASS("main_CLI_ARGUMENT_FILE_SLOT_CALLBACK");

    if (p_argument == NULL) {
        console_write_line("Invalid parameter for arguemnt -file given!");
    }

    const char* p_file_path = (const char*) p_argument;

    console_new_line();
    console_write_string("Using file: ", p_file_path);
    UF2_FILE_set_path(p_file_path);

    file_set = 1;
}

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void main_CLI_HELP_REQUESTED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;

    console_write("SHC UF2-Helper Version: ");
    console_write_line(VERSION_STR);

    console_new_line();
    console_new_line();

    console_write_line("Usage: uf2Helper [options]]\n\n");
    console_write_line("Options:");
    console_new_line();

    console_write_line("-file <path>                        : SPI-device to use for communication");
    console_write_line("-n <number>                         : Limit output to this number of blocks");
    console_new_line();
    console_new_line();

    exit_program = 1;
}

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void main_CLI_ARGUMENT_N_SLOT_CALLBACK(const void* p_argument) {

    block_count = *((const u32*)p_argument);
}

// --------------------------------------------------------------------------------
