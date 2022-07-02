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
 * @file   main_gpio_reader.c
 * @author Sebastian Lesse
 * @date   2021 / 05 / 14
 * @brief  
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
#include <fcntl.h>

// --------------------------------------------------------------------------------

#include "initialization/initialization.h"
#include "common/signal_slot_interface.h"
#include "common/common_types.h"

#include "mcu_task_management/mcu_task_controller.h"

#include "ui/command_line/command_line_interface.h"
#include "ui/command_line/command_line_handler_gpio.h"
#include "ui/console/ui_console.h"

#include "platine/board_RASPBERRYPI.h"

// --------------------------------------------------------------------------------

#include "../cfg_LINUX_GPIO_DRIVER/linux_gpio_driver_interface.h"

// --------------------------------------------------------------------------------

#define MAIN_MODULE_NAME            "GPIO Reader"

/**
 * @brief Identifier for an empty number
 * 
 */
#define NUMBER_EMPTY                255

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
static void main_CLI_ARGUMENT_GPIO_SLOT_CALLBACK(const void* p_argument);

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_HELP_REQUESTED_SIGNAL, MAIN_CLI_HELP_REQUESTED_SLOT, main_CLI_HELP_REQUESTED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_INVALID_PARAMETER_SIGNAL, MAIN_CLI_INVALID_PARAMETER_SLOT, main_CLI_INVALID_PARAMETER_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_ARGUMENT_GPIO_SIGNAL, CLI_ARGUMENT_GPIO_SLOT, main_CLI_ARGUMENT_GPIO_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

/**
 * @brief local variable that holds the cli-command data given by the user
 * Is reseted at program-start and is set via CLI_ARGUMENT_GPIO_SIGNAL
 * 
 */
static GPIO_CLI_ARGUMENT gpio_cmd;

// --------------------------------------------------------------------------------

/**
 * @brief Prints a line of the actual gpio state overview in the form
 * 
 * | bcm1 | gpio1 | name1 | mode1 | level1 | phys1 || phys2 | level2 | mode2 | name2 | gpio2 | bcm2 |
 * 
 * if bcm1 / gioPi1 / mode1 / level1 / bcm2 / gioPi2 / mode2 / level2
 * is set to NUMBER_EMPTY a white-space will be printed
 * 
 * @param bcm1 
 * @param gioPi1 
 * @param name1 
 * @param mode1 
 * @param level1 
 * @param phys1 
 * @param phys2 
 * @param level2 
 * @param mode2 
 * @param name2 
 * @param giopi2 
 * @param bcm2 
 */
static void gpio_write_line(
    uint8_t bcm1,
    uint8_t giopi1,
    const char* name1,
    uint8_t mode1,
    uint8_t level1,
    uint8_t phys1,
    uint8_t phys2,
    uint8_t level2,
    uint8_t mode2,
    const char* name2,
    uint8_t giopi2,
    uint8_t bcm2
) {

    if (bcm1 < 10) {
        console_write("|  ");
    } else {
        console_write("| ");
    }

    if (bcm1 != NUMBER_EMPTY) {
        console_write_number(bcm1);
    } else {
        console_write("  ");
    }

    if (giopi1 < 10) {
        console_write("  |   ");
    } else {
        console_write("  |  ");
    }

    if (giopi1 != NUMBER_EMPTY) {
        console_write_number(giopi1);
    } else {
        console_write("  ");
    }

    console_write("   |");
    console_write(name1);
    console_write("|");

    if (mode1 == NUMBER_EMPTY) {
        console_write("      ");
    } else if (mode1 == GPIO_DRIVER_DIRECTION_OUTPUT) {
        console_write("  OUT ");
    } else {
        console_write("   IN ");
    }

    console_write("| ");

    if (level1 == NUMBER_EMPTY) {
        console_write(" ");
    } else if (level1 == GPIO_DRIVER_LEVEL_HIGH) {
        console_write("1");
    } else {
        console_write("0");
    }

    if (phys1 < 10) {
        console_write(" |  ");
    } else {
        console_write(" | ");
    }

    console_write_number(phys1);

    console_write(" || ");
    console_write_number(phys2);

    if (phys2 < 10) {
        console_write("  | ");
    } else {
        console_write(" | ");
    }

    if (level2 == NUMBER_EMPTY) {
        console_write(" ");
    } else if (level2 == GPIO_DRIVER_LEVEL_HIGH) {
        console_write("1");
    } else {
        console_write("0");
    }

    console_write(" | ");

    if (mode2 == NUMBER_EMPTY) {
        console_write("    ");
    } else if (mode2 == GPIO_DRIVER_DIRECTION_OUTPUT) {
        console_write("OUT ");
    } else {
        console_write("IN  ");
    }

    console_write(" |");
    console_write(name2);

    if (giopi2 < 10) {
        console_write("|   ");
    } else {
        console_write("|  ");
    }

    if (giopi2 != NUMBER_EMPTY) {
        console_write_number(giopi2);
    } else {
        console_write("  ");
    }

    console_write("   | ");

    if (bcm2 != NUMBER_EMPTY) {
        console_write_number(bcm2);
    } else {
        console_write("  ");
    }

    if (bcm2 < 10) {
        console_write("   |");
    } else {
        console_write("  |");
    }

    console_new_line();
}

/**
 * @brief Prints a head/foot of the gpio overall view on the console. The output will be the following.
 * 
 * +-----+-------+---------+------+---+----------+---+------+---------+-------+-----+
 * | BCM | gioPi | Name    | Mode | V | Physical | V | Mode | Name    | gioPi | BCM |
 * +-----+-------+---------+------+---+----++----+---+------+---------+-------+-----+
 * 
 */
void gpio_write_header(void) {
    console_write("+-----+-------+---------+------+---+----------+---+------+---------+-------+-----+"); console_new_line();
    console_write("| BCM | gioPi | Name    | Mode | V | Physical | V | Mode | Name    | gioPi | BCM |"); console_new_line();
    console_write("+-----+-------+---------+------+---+----++----+---+------+---------+-------+-----+"); console_new_line();
}

// --------------------------------------------------------------------------------

/**
 * @brief Reads the actual direction and level of each gpio
 * and prints a overview on the console
 * 
 */
static void gpio_reader_read_all(void) {

    /**
     * @brief Array of read-commands
     * One command for every pin avaialble.
     * 
     */
    GPIO_DRIVER_RW_CMD my_command_array[GPIO_DRIVER_MAX_NUM_OF_GPIO_PINS];

    /**
     * @brief Read the status of all GPIOs at once
     * 
     */
    GPIO_DRIVER_OPEN(handle);
    if (handle >= 0) {

        for (u8 i = 0; i < GPIO_DRIVER_MAX_NUM_OF_GPIO_PINS; i++) {

            GPIO_DRIVER_READ_CMD( my_command_array[i], i);
            if (GPIO_DRIVER_READ(handle, my_command_array[i]) != 0) {

                console_write("Read Pin:");
                console_write_number(i);
                console_write(" has FAILED !");

                GPIO_DRIVER_CLOSE(handle);
                return;
            }
        }

        GPIO_DRIVER_CLOSE(handle);

    } else {
        console_write("Open gioPi has FAILED !");
        console_new_line();
        return;
    }

    gpio_write_header();

    gpio_write_line(
        NUMBER_EMPTY, // uint8_t bcm1,
        NUMBER_EMPTY, // uint8_t giopi1,
        " 3V3     ", //const char* name1,
        NUMBER_EMPTY, //uint8_t mode1,
        NUMBER_EMPTY, // uint8_t level1,
        1, //uint8_t phys1,
        2, //uint8_t phys2,
        NUMBER_EMPTY, // uint8_t level2,
        NUMBER_EMPTY, //uint8_t mode2,
        " 5V      ",
        NUMBER_EMPTY, //uint8_t giopi2,
        NUMBER_EMPTY //uint8_t bcm2
    );

    gpio_write_line(
        2, // uint8_t bcm1,
        0, // uint8_t giopi1,
        " SDA.1   ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[0]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[0]), // uint8_t level1,
        3, //uint8_t phys1,
        4, //uint8_t phys2,
        NUMBER_EMPTY, // uint8_t level2,
        NUMBER_EMPTY, //uint8_t mode2,
        " 5V      ",
        NUMBER_EMPTY, //uint8_t giopi2,
        NUMBER_EMPTY //uint8_t bcm2
    );

    gpio_write_line(
        3, // uint8_t bcm1,
        1, // uint8_t giopi1,
        " SCL.1   ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[1]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[1]), // uint8_t level1,
        5, //uint8_t phys1,
        6, //uint8_t phys2,
        NUMBER_EMPTY, // uint8_t level2,
        NUMBER_EMPTY, //uint8_t mode2,
        " GND     ",
        NUMBER_EMPTY, //uint8_t giopi2,
        NUMBER_EMPTY //uint8_t bcm2
    );

    gpio_write_line(
        4, // uint8_t bcm1,
        2, // uint8_t giopi1,
        " GPIO  7 ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[1]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[1]), // uint8_t level1,
        7, //uint8_t phys1,
        8, //uint8_t phys2,
        GPIO_DRIVER_GET_LEVEL(my_command_array[12]), // uint8_t level2,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[12]), //uint8_t mode2,
        " TxD     ",
        12, //uint8_t giopi2,
        14 //uint8_t bcm2
    );

    gpio_write_line(
        NUMBER_EMPTY, // uint8_t bcm1,
        NUMBER_EMPTY, // uint8_t giopi1,
        " GND     ", //const char* name1,
        NUMBER_EMPTY, //uint8_t mode1,
        NUMBER_EMPTY, // uint8_t level1,
        9, //uint8_t phys1,
        10, //uint8_t phys2,
        GPIO_DRIVER_GET_LEVEL(my_command_array[13]), // uint8_t level2,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[13]), //uint8_t mode2,
        " RxD     ",
        13, //uint8_t giopi2,
        15 //uint8_t bcm2
    );

    gpio_write_line(
        17, // uint8_t bcm1,
        15, // uint8_t giopi1,
        " GPIO  0 ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[15]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[15]), // uint8_t level1,
        11, //uint8_t phys1,
        12, //uint8_t phys2,
        GPIO_DRIVER_GET_LEVEL(my_command_array[16]), // uint8_t level2,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[16]), //uint8_t mode2,
        " GPIO  1 ",
        16, //uint8_t giopi2,
        18 //uint8_t bcm2
    );

    gpio_write_line(
        27, // uint8_t bcm1,
        25, // uint8_t giopi1,
        " GPIO  2 ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[25]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[25]), // uint8_t level1,
        13, //uint8_t phys1,
        14, //uint8_t phys2,
        NUMBER_EMPTY, // uint8_t level2,
        NUMBER_EMPTY, //uint8_t mode2,
        " GND     ",
        NUMBER_EMPTY, //uint8_t giopi2,
        NUMBER_EMPTY //uint8_t bcm2
    );

    gpio_write_line(
        22, // uint8_t bcm1,
        20, // uint8_t giopi1,
        " GPIO  3 ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[20]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[20]), // uint8_t level1,
        15, //uint8_t phys1,
        16, //uint8_t phys2,
        GPIO_DRIVER_GET_LEVEL(my_command_array[21]), // uint8_t level2,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[21]), //uint8_t mode2,
        " GPIO  4 ",
        21, //uint8_t giopi2,
        23 //uint8_t bcm2
    );

    gpio_write_line(
        NUMBER_EMPTY, // uint8_t bcm1,
        NUMBER_EMPTY, // uint8_t giopi1,
        " 3V3     ", //const char* name1,
        NUMBER_EMPTY, //uint8_t mode1,
        NUMBER_EMPTY, // uint8_t level1,
        17, //uint8_t phys1,
        18, //uint8_t phys2,
        GPIO_DRIVER_GET_LEVEL(my_command_array[22]), // uint8_t level2,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[22]), //uint8_t mode2,
        " GPIO  5 ",
        22, //uint8_t giopi2,
        24 //uint8_t bcm2
    );

    gpio_write_line(
        10, // uint8_t bcm1,
        8, // uint8_t giopi1,
        " MOSI    ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[8]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[8]), // uint8_t level1,
        19, //uint8_t phys1,
        20, //uint8_t phys2,
        NUMBER_EMPTY, // uint8_t level2,
        NUMBER_EMPTY, //uint8_t mode2,
        " GND     ",
        NUMBER_EMPTY, //uint8_t giopi2,
        NUMBER_EMPTY //uint8_t bcm2
    );

    gpio_write_line(
        9, // uint8_t bcm1,
        7, // uint8_t giopi1,
        " MISO    ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[7]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[7]), // uint8_t level1,
        21, //uint8_t phys1,
        22, //uint8_t phys2,
        GPIO_DRIVER_GET_LEVEL(my_command_array[23]), // uint8_t level2,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[23]), //uint8_t mode2,
        " GPIO  6 ",
        23, //uint8_t giopi2,
        25 //uint8_t bcm2
    );

    gpio_write_line(
        11, // uint8_t bcm1,
        9, // uint8_t giopi1,
        " SCLK    ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[9]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[9]), // uint8_t level1,
        23, //uint8_t phys1,
        25, //uint8_t phys2,
        GPIO_DRIVER_GET_LEVEL(my_command_array[6]), // uint8_t level2,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[6]), //uint8_t mode2,
        " CE0     ",
        6, //uint8_t giopi2,
        8 //uint8_t bcm2
    );

    gpio_write_line(
        NUMBER_EMPTY, // uint8_t bcm1,
        NUMBER_EMPTY, // uint8_t giopi1,
        " GND     ", //const char* name1,
        NUMBER_EMPTY, //uint8_t mode1,
        NUMBER_EMPTY, // uint8_t level1,
        25, //uint8_t phys1,
        26, //uint8_t phys2,
        GPIO_DRIVER_GET_LEVEL(my_command_array[5]), // uint8_t level2,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[5]), //uint8_t mode2,
        " RxD     ",
        5, //uint8_t giopi2,
        7 //uint8_t bcm2
    );

    gpio_write_line(
        NUMBER_EMPTY, // uint8_t bcm1,
        NUMBER_EMPTY, // uint8_t giopi1,
        " RESERVE ", //const char* name1,
        NUMBER_EMPTY, //uint8_t mode1,
        NUMBER_EMPTY, // uint8_t level1,
        27, //uint8_t phys1,
        28, //uint8_t phys2,
        NUMBER_EMPTY, // uint8_t level2,
        NUMBER_EMPTY, //uint8_t mode2,
        " RESERVE ",
        NUMBER_EMPTY, //uint8_t giopi2,
        NUMBER_EMPTY //uint8_t bcm2
    );

    gpio_write_line(
        5, // uint8_t bcm1,
        3, // uint8_t giopi1,
        " GPIO 21 ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[3]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[3]), // uint8_t level1,
        29, //uint8_t phys1,
        30, //uint8_t phys2,
        NUMBER_EMPTY, // uint8_t level2,
        NUMBER_EMPTY, //uint8_t mode2,
        " GND     ",
        NUMBER_EMPTY, //uint8_t giopi2,
        NUMBER_EMPTY //uint8_t bcm2
    );

    gpio_write_line(
        6, // uint8_t bcm1,
        4, // uint8_t giopi1,
        " GPIO 22 ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[6]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[6]), // uint8_t level1,
        31, //uint8_t phys1,
        32, //uint8_t phys2,
        GPIO_DRIVER_GET_LEVEL(my_command_array[10]), // uint8_t level2,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[10]), //uint8_t mode2,
        " GPIO 26 ",
        10, //uint8_t giopi2,
        12 //uint8_t bcm2
    );

    gpio_write_line(
        13, // uint8_t bcm1,
        11, // uint8_t giopi1,
        " GPIO 23 ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[11]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[11]), // uint8_t level1,
        33, //uint8_t phys1,
        34, //uint8_t phys2,
        NUMBER_EMPTY, // uint8_t level2,
        NUMBER_EMPTY, //uint8_t mode2,
        " GND     ",
        NUMBER_EMPTY, //uint8_t giopi2,
        NUMBER_EMPTY //uint8_t bcm2
    );

    gpio_write_line(
        19, // uint8_t bcm1,
        17, // uint8_t giopi1,
        " GPIO 23 ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[17]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[17]), // uint8_t level1,
        35, //uint8_t phys1,
        36, //uint8_t phys2,
        GPIO_DRIVER_GET_LEVEL(my_command_array[14]), // uint8_t level2,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[14]), //uint8_t mode2,
        " GPIO 27 ",
        14, //uint8_t giopi2,
        16 //uint8_t bcm2
    );

    gpio_write_line(
        26, // uint8_t bcm1,
        24, // uint8_t giopi1,
        " GPIO 25 ", //const char* name1,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[24]), //uint8_t mode1,
        GPIO_DRIVER_GET_LEVEL(my_command_array[24]), // uint8_t level1,
        37, //uint8_t phys1,
        38, //uint8_t phys2,
        GPIO_DRIVER_GET_LEVEL(my_command_array[18]), // uint8_t level2,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[18]), //uint8_t mode2,
        " GPIO 28 ",
        18, //uint8_t giopi2,
        20 //uint8_t bcm2
    );

    gpio_write_line(
        NUMBER_EMPTY, // uint8_t bcm1,
        NUMBER_EMPTY, // uint8_t giopi1,
        " GND     ", //const char* name1,
        NUMBER_EMPTY, //uint8_t mode1,
        NUMBER_EMPTY, // uint8_t level1,
        39, //uint8_t phys1,
        40, //uint8_t phys2,
        GPIO_DRIVER_GET_LEVEL(my_command_array[19]), // uint8_t level2,
        GPIO_DRIVER_GET_DIRECTION(my_command_array[19]), //uint8_t mode2,
        " GPIO 29 ",
        19, //uint8_t giopi2,
        21 //uint8_t bcm2
    );

    gpio_write_header();

    console_new_line();
}

// --------------------------------------------------------------------------------

/**
 * @brief  Get the pin-descirptor of the given pin-id.
 * If pin-id is unknow NULL is returned.
 * 
 * @param pin_id the number of the pin (BCM)
 * @return reference to the pin-descirptor of pin-id, or NULL if pin-id is unknown
 */
GPIO_DRIVER_PIN_DESCRIPTOR* pin_id_to_pin_descr(u8 pin_id) {

    switch (pin_id) {

        default: return NULL;

        case 2 : return &I2C_SDA;
        case 3 : return &I2C_SCL;
        case 4 : return &GPIO04;
        case 5 : return &GPIO05;
        case 6 : return &GPIO06;
        case 7 : return &SPI_CE1;
        case 8 : return &SPI_CE0;
        case 9 : return &SPI_MISO;
        case 10 : return &SPI_MOSI;
        case 11 : return &SPI_SCLK;
        case 12 : return &GPIO12;
        case 13 : return &GPIO13;
        case 14 : return &UART_TXD;
        case 15 : return &UART_RXD;
        case 16 : return &GPIO16;
        case 17 : return &GPIO17;
        case 18 : return &GPIO18;
        case 19 : return &GPIO19;
        case 20 : return &GPIO20;
        case 21 : return &GPIO21;
        case 22 : return &GPIO22;
        case 23 : return &GPIO23;
        case 24 : return &GPIO24;
        case 25 : return &GPIO25;
        case 26 : return &GPIO26;
        case 27 : return &GPIO27;
    }
}

// --------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

    ATOMIC_OPERATION
    (
        initialization();
    )

    // set everything to invalid
    gpio_cmd.level = CLI_HANDLER_GPIO_LEVEL_INVALID;
    gpio_cmd.mode = CLI_HANDLER_GPIO_LEVEL_INVALID;
    gpio_cmd.operation = CLI_HANDLER_OP_INVALID;
    gpio_cmd.pin = CLI_HANDLER_GPIO_PIN_INVALID;

    DEBUG_PASS("main() - MAIN_CLI_HELP_REQUESTED_SLOT_connect()");
    MAIN_CLI_HELP_REQUESTED_SLOT_connect();

    DEBUG_PASS("main() - MAIN_CLI_INVALID_PARAMETER_SLOT_connect()");
    MAIN_CLI_INVALID_PARAMETER_SLOT_connect();

    DEBUG_PASS("main() - CLI_ARGUMENT_GPIO_SLOT_connect()");
    CLI_ARGUMENT_GPIO_SLOT_connect();

    command_line_interface(argc, argv);

    if (gpio_cmd.operation == CLI_HANDLER_OP_READ) {
        
        if (gpio_cmd.pin == CLI_HANDLER_GPIO_PIN_ALL) {
            gpio_reader_read_all();
        }

    } else if (gpio_cmd.operation == CLI_HANDLER_OP_WRITE) {

        GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr = pin_id_to_pin_descr(gpio_cmd.pin);
        if (p_pin_descr == NULL) {
            console_write_line("Given Pin-Number is unknown");
            gpio_driver_deinit();
            return -1;
        }

        // in this software we want to be able to modify all of the gpio pins.
        // So if this pin was disabled by board-config, enable it anyway
        gpio_driver_activate(p_pin_descr);

        if (gpio_cmd.pin != CLI_HANDLER_GPIO_PIN_INVALID) {

            if (gpio_cmd.level != CLI_HANDLER_GPIO_LEVEL_INVALID) {

                DEBUG_PASS("main() - set level");
                gpio_driver_set_level(
                    p_pin_descr,
                    (gpio_cmd.level == CLI_HANDLER_GPIO_LEVEL_LOW) ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH
                );
            }

            if (gpio_cmd.mode != CLI_HANDLER_GPIO_DIRECTION_INVALID) {

                DEBUG_PASS("main() - set level");
                gpio_driver_set_direction(
                    p_pin_descr,
                    (gpio_cmd.mode == CLI_HANDLER_GPIO_DIRECTION_IN) ? GPIO_DIRECTION_INPUT : GPIO_DIRECTION_OUTPUT
                );
            }
        }
    }

    mcu_task_controller_terminate_all();

    gpio_driver_deinit();

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

// --------------------------------------------------------------------------------

static void main_CLI_HELP_REQUESTED_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;

    console_write("SHC GPIO-RADER Version: ");
    console_write_number(VERSION_MAJOR);
    console_write(".");
    console_write_number(VERSION_MINOR);

    console_new_line();
    console_new_line();

    console_write_line("Usage: gpioHelper <OPTIONS>\n\n");
    console_write_line("OPTIONS:");

    console_write_line("-gpio                       : <OP>:<PIN>:<DIRECTION>:<LEVEL>");
    console_write_line("                              <OP> = WRITE | READ");
    console_write_line("                              <PIN> = 0 .. 26 | ALL");
    console_write_line("                              <DIRECTION> = IN | OUT");
    console_write_line("                              <LEVEL> = HIGH | LOW");
    console_write_line("                              Examples:");
    console_write_line("                              - read all pins: -gpio READ:ALL");
    console_write_line("                              - read pin 10: -gpio READ:10");
    console_write_line("                              - set pin 11 as output with low level: -gpio WRITE:11:OUT:LOW");

    console_new_line();
    console_write_line("-h                          : Print this help screen");
}

// --------------------------------------------------------------------------------

static void main_CLI_ARGUMENT_GPIO_SLOT_CALLBACK(const void* p_argument) {

    if (p_argument == NULL) {
        console_write_line("FATAL: NULL-POINTER-EXCEPTION");
        return;
    }

    GPIO_CLI_ARGUMENT* p_gpio_cmd = (GPIO_CLI_ARGUMENT*) p_argument;

    gpio_cmd.operation = p_gpio_cmd->operation;
    gpio_cmd.pin = p_gpio_cmd->pin;
    gpio_cmd.mode = p_gpio_cmd->mode;
    gpio_cmd.level = p_gpio_cmd->level;

}

// --------------------------------------------------------------------------------
