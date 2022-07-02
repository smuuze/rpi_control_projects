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
 * @file    main_ir_board.c
 * @author  Sebastian Lesse
 * @date    2022 / 07 / 02
 * @brief   Main file of the rpi-control IR control-board
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
#include "mcu_task_management/mcu_task_controller.h"

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

    for (;;) {
        mcu_task_controller_schedule();
        task_yield();
        watchdog();
    }
}

// --------------------------------------------------------------------------------
