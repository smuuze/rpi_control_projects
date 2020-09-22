#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

///-----------------------------------------------------------------------------

#define BOARD_DESCRIPTION_FILE                          "platine/board_180920.h"
#include "platine/board_180920.h"

//-------------------------------------------------------------------------

#define config_WATCHDOG_TIMEOUT_VALUE                   WDTO_8S

//-------------------------------------------------------------------------

#define config_DEBUG_WELCOME_MESSAGE			"Welcome to RPi - Control Board V2"

//-------------------------------------------------------------------------

#define HOST_INTERFACE_SPI                                      1
#define HOST_DRIVER			                        'S'

//-------------------------------------------------------------------------

#define I2C_CLK_LIMIT 225000

//-------------------------------------------------------------------------

#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_PROTO		\
	u8 cmd_handler_version(const COMMAND_BUFFER_INTERFACE* i_cmd_buffer, const ANSWER_BUFFER_INTERFACE* i_answ_buffer);

#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_CALLBACK	\
	{CMD_VERSION, &cmd_handler_version},

//-------------------------------------------------------------------------

#define COPRO1_DRIVER_CFG		.module = {							\
						.i2c = {						\
								.is_master = 1,				\
								.bit_rate = DRIVER_I2C_BITRATE_1KHZ,	\
								.enable_ack = 1,			\
								.interrupt_enable = 1,			\
								.answer_general_call = 1,		\
								.slave_addr = 0x01			\
						}							\
					}

//-------------------------------------------------------------------------

#include "../src/config_default.h"

#endif /* _config_H_ */
