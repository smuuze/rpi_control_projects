/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	main_tracer.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

#include <stdio.h>

// --------------------------------------------------------------------------------

#include "initialization/initialization.h"

#include "common/signal_slot_interface.h"
#include "common/common_types.h"
#include "common/qeue_interface.h"

#include "mcu_task_management/mcu_task_controller.h"
#include "mcu_task_management/thread_interface.h"

#include "ui/command_line/command_line_interface.h"
#include "ui/console/ui_console.h"
#include "ui/lcd/ui_lcd_interface.h"
#include "ui/cfg_file_parser/cfg_file_parser.h"

#include "tracer/trace_object.h"
#include "app_tasks/thread_read_trace_object.h"
#include "app_tasks/thread_parse_trace_object.h"
#include "app_tasks/thread_print_trace_object.h"

// --------------------------------------------------------------------------------

#ifndef TRACER_RAW_TRACE_OBJECT_QEUE_SIZE
#define TRACER_RAW_TRACE_OBJECT_QEUE_SIZE		48
#endif

#ifndef TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE
#define TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE		48
#endif

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
static void main_CLI_UNKNOWN_ARGUMENT_SLOT_CALLBACK(const void* p_argument);

/*!
 *
 */
static void main_CLI_NO_ARGUMENT_GIVEN_CALLBACK(const void* p_argument);

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_HELP_REQUESTED_SIGNAL, MAIN_CLI_HELP_REQUESTED_SLOT, main_CLI_HELP_REQUESTED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_INVALID_PARAMETER_SIGNAL, MAIN_CLI_INVALID_PARAMETER_SLOT, main_CLI_INVALID_PARAMETER_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_UNKNOWN_ARGUMENT_SIGNAL, MAIN_CLI_UNKNOWN_ARGUMENT_SLOT, main_CLI_UNKNOWN_ARGUMENT_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_NO_ARGUMENT_GIVEN_SIGNAL, MAIN_CLI_NO_ARGUMENT_GIVEN_SLOT, main_CLI_NO_ARGUMENT_GIVEN_CALLBACK)

// --------------------------------------------------------------------------------

QEUE_INTERFACE_BUILD_QEUE(RAW_TRACE_OBJECT_QEUE, TRACE_OBJECT_RAW, sizeof(TRACE_OBJECT_RAW), TRACER_RAW_TRACE_OBJECT_QEUE_SIZE)
QEUE_INTERFACE_BUILD_QEUE(TRACE_OBJECT_QEUE, TRACE_OBJECT, sizeof(TRACE_OBJECT), TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE)

// --------------------------------------------------------------------------------

/*!
 *
 */
static u8 exit_program = 0;

// --------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

	ATOMIC_OPERATION
	(
		initialization();
	)

	DEBUG_PASS("main() - MAIN_CLI_HELP_REQUESTED_SLOT_connect()");
	MAIN_CLI_HELP_REQUESTED_SLOT_connect();

	DEBUG_PASS("main() - MAIN_CLI_INVALID_PARAMETER_SLOT_connect()");
	MAIN_CLI_INVALID_PARAMETER_SLOT_connect();

	DEBUG_PASS("main() - MAIN_CLI_UNKNOWN_ARGUMENT_SLOT_connect()");
	MAIN_CLI_UNKNOWN_ARGUMENT_SLOT_connect();

	DEBUG_PASS("main() - MAIN_CLI_NO_ARGUMENT_GIVEN_SLOT_connect()");
	MAIN_CLI_NO_ARGUMENT_GIVEN_SLOT_connect();

	printf("Welcome to the SHC TRACER v%d.%d\n\n", VERSION_MAJOR, VERSION_MINOR);

	command_line_interface(argc, argv);

	if (exit_program) {
		DEBUG_PASS("main() - PROGRAM EXIT REQUESTED !!! ---");
		return 1;
	}
	
	RAW_TRACE_OBJECT_QEUE_init();
	TRACE_OBJECT_QEUE_init();

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

static void main_CLI_INVALID_PARAMETER_SLOT_CALLBACK(const void* p_argument) {

	DEBUG_PASS("main_CLI_INVALID_PARAMETER_SLOT_CALLBACK");

	if (p_argument != NULL) {
		printf("Invalid parameter for arguemnt %s given!\n", (char*)p_argument);
	} else {
		console_write_line("Invalid parameter given, check your input!");
	}

	main_CLI_HELP_REQUESTED_SLOT_CALLBACK(NULL);
}

static void main_CLI_HELP_REQUESTED_SLOT_CALLBACK(const void* p_argument) {
	(void) p_argument;

	console_write_line("Usage: shcTracer [options]]");
	console_write_line("Options:");
	console_write_line("-path <path>                       : path to directory that includes your makefile");
	console_write_line("-file <path>                       : traceoutput will be stored into this file");
	console_write_line("-console                           : traceoutput will be shown on console");
	console_write_line("-mqtt <topic>@<servicer_ip:port>   : traceoutput will be shown on console");

	exit_program = 1;
}

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

static void main_CLI_NO_ARGUMENT_GIVEN_CALLBACK(const void* p_argument) {

	DEBUG_PASS("main_CLI_NO_ARGUMENT_GIVEN_CALLBACK()");
	console_write_line("No argument was given !");
	console_write_line("give -help to see a list of valid arguments");

	exit_program = 1;
}
