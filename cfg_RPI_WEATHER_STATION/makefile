#-----------------------------------------------------------------------------

PROJECT = RPI_Weather_Station

#-----------------------------------------------------------------------------

BOARD_ID = RASPBERRYPI

#-----------------------------------------------------------------------------

VERSION_MAJOR := 1
VERSION_MINOR := 0

#-----------------------------------------------------------------------------

BASE_PATH    = ../..
FRMWRK_PATH  = $(BASE_PATH)/rpi_control_frmwrk
#SOC_PATH     = $(BASE_PATH)/rpi_control_rpilinux
APP_PATH     = $(FRMWRK_PATH)/src
MAKE_PATH    = $(FRMWRK_PATH)/make

#-----------------------------------------------------------------------------

MCU = RASPBERRY_PI

#-----------------------------------------------------------------------------

CSRCS += main_rpi_weather_station.c

#-----------------------------------------------------------------------------

#HOST_INTERFACE_TYPE = I2C

#-----------------------------------------------------------------------------

COMMON_MODULES = 
COMMON_MODULES += QEUE
#COMMON_MODULES += DATETIME
COMMON_MODULES += STRING

#-----------------------------------------------------------------------------

APP_TASK_CFG += 
#APP_TASK_CFG += LED_MATRIX
#APP_TASK_CFG += TEST_TRACER
#APP_TASK_CFG += IR_REMOTE
#APP_TASK_CFG += EVENT
APP_TASK_CFG += KEYPAD_2_LCD

#-----------------------------------------------------------------------------

MODULES_CFG = 
MODULES_CFG += LCD_16X2
MODULES_CFG += KEYPAD_3X4
MODULES_CFG += MOVEMENT_DETECTION_MQTT

#-----------------------------------------------------------------------------

EXPANSION_BOARD_CFG = 
#EXPANSION_BOARD_CFG += SENSOR_SHT31_ADS1115
#EXPANSION_BOARD_CFG += GPIO_PCA9670

#-----------------------------------------------------------------------------

DRIVER_MODULE_CFG =
DRIVER_MODULE_CFG += GPIO_LINUX
DRIVER_MODULE_CFG += RTC
DRIVER_MODULE_CFG += CLK
#DRIVER_MODULE_CFG += IRQ
#DRIVER_MODULE_CFG += TIMER0
#DRIVER_MODULE_CFG += TIMER1
#DRIVER_MODULE_CFG += I2C0
DRIVER_MODULE_CFG += SPI0
#DRIVER_MODULE_CFG += USART0
#DRIVER_MODULE_CFG += USART1
DRIVER_MODULE_CFG += KEYPAD_3x4
DRIVER_MODULE_CFG += THE_PI_HUT_PIR
DRIVER_MODULE_CFG += DEVICE_NAME_SUPPORT

#-----------------------------------------------------------------------------

PROTOCOL_CFG =
#PROTOCOL_CFG += RPI_PROTOCOL_I2C
PROTOCOL_CFG += MQTT_PROTOCOL
PROTOCOL_CFG += JSON_PARSER

#-----------------------------------------------------------------------------

SENSOR_MODULE_CFG =
#SENSOR_MODULE_CFG += LIGHT_SENSOR_GM5528

#-----------------------------------------------------------------------------

MANAGEMENT_MODULE_CFG =
#MANAGEMENT_MODULE_CFG += POWER
#MANAGEMENT_MODULE_CFG += IO
#MANAGEMENT_MODULE_CFG += RPI_PROTOCOL_I2C
#MANAGEMENT_MODULE_CFG += RPI_PROTOCOL
MANAGEMENT_MODULE_CFG += TIMER

#-----------------------------------------------------------------------------

COMMAND_INTERFACE_CFG = 
#COMMAND_INTERFACE_CFG += RPI_CMD_IR_REMOTE_SAMSUNG
#COMMAND_INTERFACE_CFG += RPI_CMD_IR_REMOTE_JVC
#COMMAND_INTERFACE_CFG += RPI_CMD_IR_REMOTE_SONY
#COMMAND_INTERFACE_CFG += RPI_CMD_IR_REMOTE_LED_LIGHTS

#-----------------------------------------------------------------------------

USER_INTERFACE_CFG =
USER_INTERFACE_CFG += CFG_FILE_PARSER
USER_INTERFACE_CFG += CLI_ARG_DEVICE
# USER_INTERFACE_CFG += CLI_ARG_COMMAND
# USER_INTERFACE_CFG += CONSOLE
USER_INTERFACE_CFG += FILE_INTERFACE
# USER_INTERFACE_CFG += LOG_INTERFACE

#-----------------------------------------------------------------------------

TRACER_CFG =
TRACER_CFG += CONSOLE
# TRACER_CFG += USART0
#TRACER_CFG += BAUDRATE_230400
# TRACER_CFG += BBAUDRATE_115200
#TRACER_CFG += BAUDRATE_9600
#TRACER_CFG += PARITY_NONE
#TRACER_CFG += DATABITS_8
#TRACER_CFG += STOPBITS_1

#-----------------------------------------------------------------------------
# Fuer alle Projekte gueltige Dateien
include $(MAKE_PATH)/common_make.mk
