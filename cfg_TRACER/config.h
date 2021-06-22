#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

///-----------------------------------------------------------------------------

#define BOARD_DESCRIPTION_FILE                          "platine/board_RASPBERRYPI.h"
#include "platine/board_RASPBERRYPI.h"

//-------------------------------------------------------------------------

#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_PROTO		\
	u8 cmd_handler_version(const COMMAND_BUFFER_INTERFACE* i_cmd_buffer, const ANSWER_BUFFER_INTERFACE* i_answ_buffer);

#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_CALLBACK	\
	{CMD_VERSION, &cmd_handler_version},

//-------------------------------------------------------------------------

#define USART0_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER			2048
#define USART0_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER			2048

//-------------------------------------------------------------------------

#include "../src/config_default.h"

#endif /* _config_H_ */
