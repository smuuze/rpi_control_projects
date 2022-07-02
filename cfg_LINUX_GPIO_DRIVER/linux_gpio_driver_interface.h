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
 * @file    linux_gpio_driver_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 05 / 14
 * @brief   Short description of this file
 * 
 * Usage for write
 * 
 *      GPIO_DRIVER_OPEN(my_handle);
 *      if (my_handle < 0) {
 *          ...
 *      }
 * 
 *      GPIO_DRIVER_CREATE_COMMAND(my_write_cmd);
 *      GPIO_DRIVER_WRITE_CMD(
 *          my_write_cmd,
 *          GPIO_DRIVER_GPIO_15,
 *          GPIO_DRIVER_DIRECTION_OUTPUT,
 *          GPIO_DRIVER_LEVEL_HIGH
 *      );
 * 
 *      if (GPIO_DRIVER_READ(my_handle, my_write_cmd) != 0) {
 *          ...
 *      }
 * 
 *      GPIO_DRIVER_CLOSE(my_handle);
 * 
 * Usage for read
 * 
 *      GPIO_DRIVER_OPEN(my_handle);
 *      if (my_handle < 0) {
 *          ...
 *      }
 * 
 *      GPIO_DRIVER_CREATE_COMMAND(my_read_cmd);
 *      GPIO_DRIVER_READ_CMD(my_read_cmd, GPIO_DRIVER_GPIO_15);
 * 
 *      if (GPIO_DRIVER_READ(my_handle, my_read_cmd) != 0) {
 *          ...
 *      }
 * 
 *      if (GPIO_DRIVER_GET_LEVEL(my_read_cmd) == GPIO_DRIVER_LEVEL_HIGH) {
 *          ...
 *      }
 * 
 *      GPIO_DRIVER_CLOSE(my_handle); 
 * 
 * ---------------------------------------------------------------------------------
 * 
 *          GPIO Mapping:
 * 
 *          - The GPIO_DRIVER has a offset of 2 acording to the GPIO XX number
 *          - There is a maximum of 26 GPIOs that can be used on a Raspberry Pi
 * 
 *          --------------
 *          | GPIO-DRIVER | FUNC / ALT          | PIN  ||  PIN | FUNC / ALT          | GPIO-DRIVER |
 *          |             | +3V3                |  01  ||  02  | +5V                 |             |
 *          |      0      | GPIO 2 / I2C0 SDA   |  03  ||  04  | +5V                 |             |
 *          |      1      | GPIO 3 / I2C0 SCL   |  05  ||  06  | GND                 |             |
 *          |      2      | GPIO 4              |  07  ||  08  | GPIO 14 / UART0 TX  |     12      |
 *          |             | GND                 |  09  ||  10  | GPIO 15 / UART0 RX  |     13      |
 *          |      15     | GPIO 17             |  11  ||  12  | GPIO 18             |     16      |
 *          |      25     | GPIO 27             |  13  ||  14  | GND                 |             |
 *          |      20     | GPIO 22             |  15  ||  16  | GPIO 23             |     21      |
 *          |             | +3V3                |  17  ||  18  | GPIO 24             |     22      |
 *          |      8      | GPIO 10 / SPI0 MOSI |  19  ||  20  | GND                 |             |
 *          |      7      | GPIO 9  / SPI0 MISO |  21  ||  22  | GPIO 25             |     23      |
 *          |      9      | GPIO 11 / SPI0 SCLK |  23  ||  24  | GPIO 8 / SPI0 CS0   |     6       |
 *          |             | GND                 |  25  ||  26  | GPIO 7 / SPI0 CS1   |     5       |
 *          |             | RESERVED            |  27  ||  28  | RESERVED            |             |
 *          |      3      | GPIO 5              |  29  ||  30  | GND                 |             |
 *          |      4      | GPIO 6              |  31  ||  32  | GPIO 12             |     10      |
 *          |     11      | GPIO 13             |  33  ||  34  | GND                 |             |
 *          |     17      | GPIO 19 / SPI1 MISO |  35  ||  36  | GPIO 16 / SPI1 CS0  |     14      |
 *          |     24      | GPIO 26 / SPI0 MISO |  37  ||  38  | GPIO 20 / SPI1 MOSI |     18      |
 *          |             | GND                 |  39  ||  40  | GPIO 21 / SPI1 SCLK |     19      |
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_linux_gpio_driver_interface_
#define _H_linux_gpio_driver_interface_

// --------------------------------------------------------------------------------

/**
 * @brief Available GPIO numbers
 * The name of the macro represents the pin number of the Raspberry Pi's
 * GPIO header and the value of the macro is the gpio-number used by the GPIO-DRIVER
 * 
 */

#define GPIO_DRIVER_GPIO_02      0
#define GPIO_DRIVER_GPIO_03      1
#define GPIO_DRIVER_GPIO_04      2
#define GPIO_DRIVER_GPIO_05      3
#define GPIO_DRIVER_GPIO_06      4
#define GPIO_DRIVER_GPIO_07      5
#define GPIO_DRIVER_GPIO_08      6
#define GPIO_DRIVER_GPIO_09      7
#define GPIO_DRIVER_GPIO_10      8
#define GPIO_DRIVER_GPIO_11      9
#define GPIO_DRIVER_GPIO_12     10
#define GPIO_DRIVER_GPIO_13     11
#define GPIO_DRIVER_GPIO_14     12
#define GPIO_DRIVER_GPIO_15     13
#define GPIO_DRIVER_GPIO_16     14
#define GPIO_DRIVER_GPIO_17     15
#define GPIO_DRIVER_GPIO_18     16
#define GPIO_DRIVER_GPIO_19     17
#define GPIO_DRIVER_GPIO_20     18
#define GPIO_DRIVER_GPIO_21     19
#define GPIO_DRIVER_GPIO_22     20
#define GPIO_DRIVER_GPIO_23     21
#define GPIO_DRIVER_GPIO_24     22
#define GPIO_DRIVER_GPIO_25     23
#define GPIO_DRIVER_GPIO_26     24
#define GPIO_DRIVER_GPIO_27     25

/**
 * @brief The maximum number of gpios
 * that are supported by this driver
 * 
 */
#define GPIO_DRIVER_MAX_NUM_OF_GPIO_PINS        27

// --------------------------------------------------------------------------------

/**
 * @brief Used inside of a gpio-write command
 * to keep the direction unchanged
 * 
 */
#define GPIO_DRIVER_DIRECTION_UNCHANGED         0x00

/**
 * @brief Used inside of a gpio-write command
 * to change the direction of a gpio to output
 * 
 */
#define GPIO_DRIVER_DIRECTION_OUTPUT            0x01

/**
 * @brief Used inside of a gpio-write command
 * to change the direction of a gpio to input
 * 
 */
#define GPIO_DRIVER_DIRECTION_INPUT             0x02

/**
 * @brief Used inside of a gpio-write command
 * to toggle the direction of a gpio
 * 
 */
#define GPIO_DRIVER_DIRECTION_TOGGLE            0x03

/**
 * @brief Used inside of a gpio-write command
 * to keep the level unchanged
 * 
 */
#define GPIO_DRIVER_LEVEL_UNCHANGED             0x00

/**
 * @brief Used inside of a gpio-write command
 * to change the level of a gpio to high
 * 
 */
#define GPIO_DRIVER_LEVEL_HIGH                  0x01

/**
 * @brief Used inside of a gpio-write command
 * to change the level of a gpio to low
 * 
 */
#define GPIO_DRIVER_LEVEL_LOW                   0x02

/**
 * @brief Used inside of a gpio-write command
 * to toggle the level of a gpio
 * 
 */
#define GPIO_DRIVER_LEVEL_TOGGLE                0x03

// --------------------------------------------------------------------------------

/**
 * @brief Get the actual gpio-level from a GPIO-DRIVERS command
 * 
 */
#define GPIO_DRIVER_GET_LEVEL(cmd)      cmd.gpio_level

/**
 * @brief Get the actual gpio-direction from a GPIO-DRIVERS command
 * 
 */
#define GPIO_DRIVER_GET_DIRECTION(cmd)  cmd.gpio_direction

// --------------------------------------------------------------------------------

/**
 * @brief Opens the GPIO-DRIVER and generates a handle for read/write operations
 * 
 */
#define GPIO_DRIVER_OPEN(handle)        int handle = open("/dev/GPIO_DRIVER", O_RDWR)

/**
 * @brief Performs a read operation on the actual instance of the GPIO-DRIVER
 * 
 */
#define GPIO_DRIVER_READ(handle, cmd)   read(handle, &cmd, sizeof(GPIO_DRIVER_RW_CMD))

/**
 * @brief Performs a read operation on the actual instance of the GPIO-DRIVER
 * 
 */
#define GPIO_DRIVER_WRITE(handle, cmd)  write(handle, &cmd, sizeof(GPIO_DRIVER_RW_CMD))

/**
 * @brief Closes an instance of the GPIO-DRIVER
 * and invalidates the given handle.
 * 
 */
#define GPIO_DRIVER_CLOSE(handle)       close(handle); handle = -1

// --------------------------------------------------------------------------------

/**
 * @brief Use inside of driver_write/driver_read to set/get the
 * direction and level of a gpio-pin
 * 
 */
typedef struct GPIO_DRIVER_RW_CMD_STRUCT {

    /**
     * @brief The gpio-pin number that will be modified/read
     * 
     */
    uint8_t gpio_number;

    /**
     * @brief new/actual dircetion of the gpio-pin
     * @see GPIO_DRIVER_DIRECTION_OUTPUT,
     * GPIO_DRIVER_DIRECTION_INPUT,
     * GPIO_DRIVER_DIRECTION_TOGGLE
     */
    uint8_t gpio_direction;

    /**
     * @brief New/actual level of the gpio-pin
     * 
     */
    uint8_t gpio_level;

    /**
     * @brief Reserved for future use.
     * Do not use!
     * 
     */
    uint8_t rfu;

} GPIO_DRIVER_RW_CMD;

// --------------------------------------------------------------------------------

/**
 * @brief Helper macro to create a read / write command
 * The command is left empty. It must be filled with 
 * GPIO_DRIVER_READ_CMD() or GPIO_DRIVER_WRITE_CMD()
 * 
 */
#define GPIO_DRIVER_CREATE_COMMAND(cmd_name)    GPIO_DRIVER_RW_CMD cmd_name

/**
 * @brief Helper macro to create a read-command that can be used for the GPIO-DRIVER read function
 * 
 */
#define GPIO_DRIVER_READ_CMD(cmd_name, gpio)    cmd_name.gpio_number = gpio;                                \
                                                cmd_name.gpio_direction = GPIO_DRIVER_DIRECTION_UNCHANGED;  \
                                                cmd_name.gpio_level = GPIO_DRIVER_LEVEL_UNCHANGED;           \
                                                cmd_name.rfu = 0

/**
 * @brief Helper macro to create a write-command that can be used for the GPIO-DRIVER read function
 * 
 */
#define GPIO_DRIVER_WRITE_CMD(cmd_name, gpio, direction, level) cmd_name.gpio_number = gpio;            \
                                                                cmd_name.gpio_direction = direction;    \
                                                                cmd_name.gpio_level = level;            \
                                                                cmd_name.rfu = 0

// --------------------------------------------------------------------------------

#endif // _H_linux_gpio_driver_interface_

// --------------------------------------------------------------------------------
