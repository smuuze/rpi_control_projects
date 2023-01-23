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
PROJECT = RPI_Hat_IR_Board

BOARD_ID = 191005

#-----------------------------------------------------------------------------

VERSION_MAJOR		:= 1
VERSION_MINOR		:= 11

#-----------------------------------------------------------------------------

BASE_PATH   = ../..
FRMWRK_PATH = $(BASE_PATH)/rpi_control_frmwrk
APP_PATH    = $(FRMWRK_PATH)/src
MAKE_PATH   = $(FRMWRK_PATH)/make

#-----------------------------------------------------------------------------

MCU = atmega1284p

#-----------------------------------------------------------------------------

CSRCS += main_ir_board.c

#-----------------------------------------------------------------------------

HOST_INTERFACE_TYPE = I2C

#-----------------------------------------------------------------------------

COMMON_MODULES = 
COMMON_MODULES += QEUE
#COMMON_MODULES += DATETIME
#COMMON_MODULES += STRING

#-----------------------------------------------------------------------------

APP_TASK_CFG += 
#APP_TASK_CFG += LED_MATRIX
#APP_TASK_CFG += TEST_TRACER
APP_TASK_CFG += EVENT

#-----------------------------------------------------------------------------

MODULES_CFG = 
MODULES_CFG += IR_PROTOCOL_SONY
MODULES_CFG += IR_PROTOCOL_NEC
MODULES_CFG += IR_PROTOCOL_JVC
MODULES_CFG += IR_PROTOCOL_SAMSUNG
MODULES_CFG += IR_COMMAND_LED_LIGHT

#-----------------------------------------------------------------------------

EXPANSION_BOARD_CFG = 
#EXPANSION_BOARD_CFG += SENSOR_SHT31_ADS1115
#EXPANSION_BOARD_CFG += GPIO_PCA9670

#-----------------------------------------------------------------------------

DRIVER_MODULE_CFG =
DRIVER_MODULE_CFG += GPIO
DRIVER_MODULE_CFG += RTC
DRIVER_MODULE_CFG += CLK
DRIVER_MODULE_CFG += TIMER0
DRIVER_MODULE_CFG += TIMER1
DRIVER_MODULE_CFG += I2C0
#DRIVER_MODULE_CFG += USART0
#DRIVER_MODULE_CFG += USART1

#-----------------------------------------------------------------------------

PROTOCOL_CFG =
PROTOCOL_CFG += RPI_PROTOCOL_I2C

#-----------------------------------------------------------------------------

SENSOR_MODULE_CFG =
#SENSOR_MODULE_CFG += LIGHT_SENSOR_GM5528

#-----------------------------------------------------------------------------

MANAGEMENT_MODULE_CFG =
MANAGEMENT_MODULE_CFG += TIMER
#MANAGEMENT_MODULE_CFG += POWER
MANAGEMENT_MODULE_CFG += IO
MANAGEMENT_MODULE_CFG += RPI_PROTOCOL_I2C
#MANAGEMENT_MODULE_CFG += RPI_PROTOCOL

#-----------------------------------------------------------------------------

COMMAND_INTERFACE_CFG = 

#-----------------------------------------------------------------------------

TRACER_CFG =
TRACER_CFG += USART0
TRACER_CFG += BAUDRATE_230400
#TRACER_CFG += PARITY_NONE
#TRACER_CFG += DATABITS_8
#TRACER_CFG += STOPBITS_1

#-----------------------------------------------------------------------------
# Fuer alle Projekte gueltige Dateien
include $(MAKE_PATH)/common_make.mk
