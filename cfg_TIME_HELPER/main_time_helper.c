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
 * @file   main_time_helper.c
 * @author Sebastian Lesse
 * @date   2021 / 02 / 20
 * @brief  SHC Time Helper main source file
 *          This program is used to test signedness of time values.
 *          The program does not take any command line arguments
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
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U8(MAIN_U8_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U16(MAIN_U16_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U32(MAIN_U32_TIMER)

// --------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

	(void) argc;
	(void) argv;

	ATOMIC_OPERATION
	(
		initialization();
	)

	MAIN_U8_TIMER_start();
	MAIN_U16_TIMER_start();
	MAIN_U32_TIMER_start();

	u8  start_time_u8  = MAIN_U8_TIMER_start_time();
	u16 start_time_u16 = MAIN_U16_TIMER_start_time();
	u32 start_time_u32 = MAIN_U32_TIMER_start_time();

	printf("Start-Time u8  - Signed: %d - Unsigned %u \n", start_time_u8,  start_time_u8);
	printf("Start-Time u16 - Signed: %d - Unsigned %u \n", start_time_u16, start_time_u16);
	printf("Start-Time u32 - Signed: %d - Unsigned %u \n", start_time_u32, start_time_u32);

	return 0;
}
