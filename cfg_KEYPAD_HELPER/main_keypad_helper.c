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
#include "common/common_tools_string.h"

#include "mcu_task_management/mcu_task_controller.h"

#include "ui/console/ui_console.h"

#include "modules/keypad/keypad_interface.h"

// --------------------------------------------------------------------------------

#define MAIN_MODULE_NAME            "KEYPAD Helper"

// --------------------------------------------------------------------------------

static void main_KEY_0_PRESSED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_0 PRESSED");
}

static void main_KEY_0_DOWN_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_0 DOWN");
}

static void main_KEY_0_RELEASED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_0 RELEASE");
}


SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_0_PRESSED, MAIN_KEY_0_PRESSED_SLOT, main_KEY_0_PRESSED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_0_DOWN, MAIN_KEY_0_DOWN_SLOT, main_KEY_0_DOWN_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_0_RELEASED, MAIN_KEY_0_RELEASED_SLOT, main_KEY_0_RELEASED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

static void main_KEY_1_PRESSED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_1 PRESSED");
}

static void main_KEY_1_DOWN_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_1 DOWN");
}

static void main_KEY_1_RELEASED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_1 RELEASE");
}


SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_1_PRESSED, MAIN_KEY_1_PRESSED_SLOT, main_KEY_1_PRESSED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_1_DOWN, MAIN_KEY_1_DOWN_SLOT, main_KEY_1_DOWN_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_1_RELEASED, MAIN_KEY_1_RELEASED_SLOT, main_KEY_1_RELEASED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

static void main_KEY_2_PRESSED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_2 PRESSED");
}

static void main_KEY_2_DOWN_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_2 DOWN");
}

static void main_KEY_2_RELEASED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_2 RELEASE");
}


SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_2_PRESSED, MAIN_KEY_2_PRESSED_SLOT, main_KEY_2_PRESSED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_2_DOWN, MAIN_KEY_2_DOWN_SLOT, main_KEY_2_DOWN_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_2_RELEASED, MAIN_KEY_2_RELEASED_SLOT, main_KEY_2_RELEASED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

static void main_KEY_3_PRESSED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_3 PRESSED");
}

static void main_KEY_3_DOWN_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_3 DOWN");
}

static void main_KEY_3_RELEASED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_3 RELEASE");
}


SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_3_PRESSED, MAIN_KEY_3_PRESSED_SLOT, main_KEY_3_PRESSED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_3_DOWN, MAIN_KEY_3_DOWN_SLOT, main_KEY_3_DOWN_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_3_RELEASED, MAIN_KEY_3_RELEASED_SLOT, main_KEY_3_RELEASED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

static void main_KEY_4_PRESSED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_4 PRESSED");
}

static void main_KEY_4_DOWN_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_4 DOWN");
}

static void main_KEY_4_RELEASED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_4 RELEASE");
}


SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_4_PRESSED, MAIN_KEY_4_PRESSED_SLOT, main_KEY_4_PRESSED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_4_DOWN, MAIN_KEY_4_DOWN_SLOT, main_KEY_4_DOWN_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_4_RELEASED, MAIN_KEY_4_RELEASED_SLOT, main_KEY_4_RELEASED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

static void main_KEY_5_PRESSED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_5 PRESSED");
}

static void main_KEY_5_DOWN_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_5 DOWN");
}

static void main_KEY_5_RELEASED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_5 RELEASE");
}


SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_5_PRESSED, MAIN_KEY_5_PRESSED_SLOT, main_KEY_5_PRESSED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_5_DOWN, MAIN_KEY_5_DOWN_SLOT, main_KEY_5_DOWN_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_5_RELEASED, MAIN_KEY_5_RELEASED_SLOT, main_KEY_5_RELEASED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

static void main_KEY_6_PRESSED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_6 PRESSED");
}

static void main_KEY_6_DOWN_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_6 DOWN");
}

static void main_KEY_6_RELEASED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_6 RELEASE");
}


SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_6_PRESSED, MAIN_KEY_6_PRESSED_SLOT, main_KEY_6_PRESSED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_6_DOWN, MAIN_KEY_6_DOWN_SLOT, main_KEY_6_DOWN_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_6_RELEASED, MAIN_KEY_6_RELEASED_SLOT, main_KEY_6_RELEASED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

static void main_KEY_7_PRESSED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_7 PRESSED");
}

static void main_KEY_7_DOWN_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_7 DOWN");
}

static void main_KEY_7_RELEASED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_7 RELEASE");
}


SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_7_PRESSED, MAIN_KEY_7_PRESSED_SLOT, main_KEY_7_PRESSED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_7_DOWN, MAIN_KEY_7_DOWN_SLOT, main_KEY_7_DOWN_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_7_RELEASED, MAIN_KEY_7_RELEASED_SLOT, main_KEY_7_RELEASED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

static void main_KEY_8_PRESSED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_8 PRESSED");
}

static void main_KEY_8_DOWN_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_8 DOWN");
}

static void main_KEY_8_RELEASED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_8 RELEASE");
}


SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_8_PRESSED, MAIN_KEY_8_PRESSED_SLOT, main_KEY_8_PRESSED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_8_DOWN, MAIN_KEY_8_DOWN_SLOT, main_KEY_8_DOWN_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_8_RELEASED, MAIN_KEY_8_RELEASED_SLOT, main_KEY_8_RELEASED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

static void main_KEY_9_PRESSED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_9 PRESSED");
}

static void main_KEY_9_DOWN_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_9 DOWN");
}

static void main_KEY_9_RELEASED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    console_write_line("KEY_9 RELEASE");
}


SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_9_PRESSED, MAIN_KEY_9_PRESSED_SLOT, main_KEY_9_PRESSED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_9_DOWN, MAIN_KEY_9_DOWN_SLOT, main_KEY_9_DOWN_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_9_RELEASED, MAIN_KEY_9_RELEASED_SLOT, main_KEY_9_RELEASED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------


/**
 * @brief The program will be exit if set to 1.
 * 
 */
static u8 exit_program = 0;

// --------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

    ATOMIC_OPERATION
    (
        initialization();
    )

    MAIN_KEY_0_PRESSED_SLOT_connect();
    MAIN_KEY_0_DOWN_SLOT_connect();
    MAIN_KEY_0_RELEASED_SLOT_connect();

    MAIN_KEY_1_PRESSED_SLOT_connect();
    MAIN_KEY_1_DOWN_SLOT_connect();
    MAIN_KEY_1_RELEASED_SLOT_connect();

    MAIN_KEY_2_PRESSED_SLOT_connect();
    MAIN_KEY_2_DOWN_SLOT_connect();
    MAIN_KEY_2_RELEASED_SLOT_connect();

    MAIN_KEY_3_PRESSED_SLOT_connect();
    MAIN_KEY_3_DOWN_SLOT_connect();
    MAIN_KEY_3_RELEASED_SLOT_connect();

    MAIN_KEY_4_PRESSED_SLOT_connect();
    MAIN_KEY_4_DOWN_SLOT_connect();
    MAIN_KEY_4_RELEASED_SLOT_connect();

    MAIN_KEY_5_PRESSED_SLOT_connect();
    MAIN_KEY_5_DOWN_SLOT_connect();
    MAIN_KEY_5_RELEASED_SLOT_connect();

    MAIN_KEY_6_PRESSED_SLOT_connect();
    MAIN_KEY_6_DOWN_SLOT_connect();
    MAIN_KEY_6_RELEASED_SLOT_connect();

    MAIN_KEY_7_PRESSED_SLOT_connect();
    MAIN_KEY_7_DOWN_SLOT_connect();
    MAIN_KEY_7_RELEASED_SLOT_connect();

    MAIN_KEY_8_PRESSED_SLOT_connect();
    MAIN_KEY_8_DOWN_SLOT_connect();
    MAIN_KEY_8_RELEASED_SLOT_connect();

    MAIN_KEY_9_PRESSED_SLOT_connect();
    MAIN_KEY_9_DOWN_SLOT_connect();
    MAIN_KEY_9_RELEASED_SLOT_connect();

    command_line_interface(argc, argv);

    for (;;) {
        
        mcu_task_controller_schedule();
        mcu_task_controller_background_run();
        watchdog();

        if (exit_program) {
            break;
        }
    }

    mcu_task_controller_terminate_all();

    ATOMIC_OPERATION
    (
        deinitialization();
    )

    return 0;
}

// --------------------------------------------------------------------------------
