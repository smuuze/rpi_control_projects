#-----------------------------------------------------------------------------
#       Makefile fuer AVR-GCC Projekte
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# PROJECT: Name des Hexfiles usw.
#-----------------------------------------------------------------------------
# VERSION_STRING muss (!) zwei Ziffern nach dem '.' enthalten, weil
# aus dem String noch weitere Varianten von Version erzeugt
# werden. Beispiel:
# VERSION_STRING = v1.07
# VERSION_DIGITS = 107
# VERSION        = 0x107

# PROJEKT Name des Hexfiles
PROJECT = RPI_Hat_ControlBoard_V2

BOARD_ID = 180920

#-----------------------------------------------------------------------------

VERSION_MAJOR		:= 4
VERSION_MINOR		:= 12

#-----------------------------------------------------------------------------

BASE_PATH   = ../../rpi_control_frmwrk
APP_PATH    = $(BASE_PATH)/src
MAKE_PATH   = $(BASE_PATH)/make

#-----------------------------------------------------------------------------

MCU = ATMEGA1284P

#-----------------------------------------------------------------------------

CSRCS += ${APP_PATH}/main_rpi_hat.c

#-----------------------------------------------------------------------------

HOST_INTERFACE_TYPE = SPI

#-----------------------------------------------------------------------------

APP_TASK_CFG = 
APP_TASK_CFG += EVENT
APP_TASK_CFG += COPRO_ROUTING
#APP_TASK_CFG += LED_MATRIX
#APP_TASK_CFG += TEST_TRACER

#-----------------------------------------------------------------------------

EXPANSION_BOARD_CFG = 
EXPANSION_BOARD_CFG += SENSOR_SHT31_ADS1115
EXPANSION_BOARD_CFG += GPIO_PCA9670

#-----------------------------------------------------------------------------

DRIVER_MODULE_CFG =
DRIVER_MODULE_CFG += GPIO
DRIVER_MODULE_CFG += RTC
DRIVER_MODULE_CFG += CLK
DRIVER_MODULE_CFG += USART0
DRIVER_MODULE_CFG += I2C0
DRIVER_MODULE_CFG += SPI0

#-----------------------------------------------------------------------------

SENSOR_MODULE_CFG =
SENSOR_MODULE_CFG += LIGHT_SENSOR_GM5528

#-----------------------------------------------------------------------------

MANAGEMENT_MODULE_CFG =
MANAGEMENT_MODULE_CFG += POWER
MANAGEMENT_MODULE_CFG += IO
MANAGEMENT_MODULE_CFG += RPI_PROTOCOL

#-----------------------------------------------------------------------------

COMMAND_INTERFACE_CFG = 
COMMAND_INTERFACE_CFG += RPI_CMD_IO
COMMAND_INTERFACE_CFG += RPI_CMD_SENSOR
COMMAND_INTERFACE_CFG += RPI_CMD_ROUTING

#-----------------------------------------------------------------------------

COPRO_INTERFACE_CFG =
COPRO_INTERFACE_CFG += COPRO1_I2C0

#-----------------------------------------------------------------------------

TRACER_CFG =
TRACER_CFG += USART1
TRACER_CFG += BAUDRATE_230400
#TRACER_CFG += PARITY_NONE
#TRACER_CFG += DATABITS_8
#TRACER_CFG += STOPBITS_1

#-----------------------------------------------------------------------------
# Fuer alle Projekte gueltige Dateien
include $(MAKE_PATH)/common_make.mk
