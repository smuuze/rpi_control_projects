#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

///-----------------------------------------------------------------------------

#define BOARD_DESCRIPTION_FILE			"platine/board_191005.h"
#include "platine/board_191005.h"

//-------------------------------------------------------------------------

#define config_WATCHDOG_TIMEOUT_VALUE		WDTO_8S

//-------------------------------------------------------------------------

#define HOST_INTERFACE_I2C			1
#define HOST_DRIVER				'I'

//-------------------------------------------------------------------------

#define DISABLE_SLEEP_ON_IDLE			1

//-------------------------------------------------------------------------

#define I2C_CLK_LIMIT 225000

//-------------------------------------------------------------------------

#define config_DEBUG_WELCOME_MESSAGE		"Welcome to RPi - IR-BOARD V1"

//-------------------------------------------------------------------------

#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_PROTO		\
	u8 cmd_handler_version(const COMMAND_BUFFER_INTERFACE* i_cmd_buffer, const ANSWER_BUFFER_INTERFACE* i_answ_buffer);

#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_CALLBACK	\
	{CMD_VERSION, &cmd_handler_version},

//-------------------------------------------------------------------------

#include "../src/config_default.h"

#endif /* _config_H_ */
