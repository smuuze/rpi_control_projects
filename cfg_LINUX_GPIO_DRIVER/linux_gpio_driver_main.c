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
 * @file    linux_gpio_driver_main.c
 * @author  Sebastian Lesse
 * @date    2022 / 05 / 07
 * @brief   A simple gpio driver useable on a Raspberry Pi.
 * 
 *          Usage:
 * 
 *          1. Open the driver
 * 
 *          2. Set direction of GPIO
 * 
 *          3. Set Level of GPIO
 * 
 *          4. Set level and direction of GPIO
 * 
 *          5. Toggle level of GPIO
 * 
 *          6. Read actual level of GPIO
 *
 * 
 * @see https://www.kernel.org/doc/html/latest/driver-api/gpio/consumer.html
 * @see https://www.kernel.org/doc/html/latest/driver-api/gpio/board.html
 * @see https://www.kernel.org/doc/html/latest/driver-api/gpio/index.html#c.gpiod_get_index
 * @see https://www.linux-magazin.de/ausgaben/2019/01/kern-technik-6/4/
 * @see https://www.codeproject.com/Articles/1032794/Simple-I-O-device-driver-for-RaspberryPi
 */

// --------------------------------------------------------------------------------

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/gpio/consumer.h>
#include <linux/slab.h>
#include <linux/gpio.h>

#include <asm/io.h>
#include <linux/ioport.h>

#include <asm/uaccess.h>

// --------------------------------------------------------------------------------

#include "linux_gpio_driver_interface.h"

// --------------------------------------------------------------------------------

// anem of this driver
#define DRIVER_NAME "GPIO_DRIVER"

// --------------------------------------------------------------------------------

#define PRINT_MSG(...) printk("[" DRIVER_NAME "] " __VA_ARGS__)

// --------------------------------------------------------------------------------

#define GPIO_PORT_ADDR                          0x20200000

// --------------------------------------------------------------------------------

#define GPIO_PORT_RANGE                         0x40

// --------------------------------------------------------------------------------

/**
 * @brief Identifies an invalid gpio number
 * that was gieven by th user-application
 * 
 */
#define GPIO_DRIVER_GPIO_NUM_INVALID            -1

/**
 * @brief Indicates a gpio-pin in the
 * gpio-array of a driver-instanace as unused
 * 
 */
#define GPIO_DRIVER_PIN_UNUSED                  0xFF

/**
 * @brief  Indicates a gpio-pin in the
 * gpio-array of a driver-instanace as ready to use
 * 
 */
#define GPIO_DRIVER_PIN_READY                   0x00

/**
 * @brief  Indicates a gpio-pin in the
 * gpio-array of a driver-instanace as output
 * 
 */
#define GPIO_DRIVER_PIN_IS_OUTPUT               0x10

/**
 * @brief  Indicates a gpio-pin in the
 * gpio-array of a driver-instanace as input
 * 
 */
#define GPIO_DRIVER_PIN_IS_INPUT                0x30

/**
 * @brief  Indicates a gpio-pin in the
 * gpio-array of a driver-instanace is set to high
 * 
 */
#define GPIO_DRIVER_PIN_IS_HIGH                 0x01

/**
 * @brief  Indicates a gpio-pin in the
 * gpio-array of a driver-instanace is set to low
 * 
 */
#define GPIO_DRIVER_PIN_IS_LOW                  0x03

// --------------------------------------------------------------------------------

/**
 * @brief Helper Macro to check if the
 * GPIO is actual configrued as output
 * 
 */
#define GPIO_STATUS_IS_OUTPUT(status)           ((status & 0x30) == GPIO_DRIVER_PIN_IS_OUTPUT)
#define GPIO_STATUS_SET_OUTPUT(status)          status = (status & 0xCF) | GPIO_DRIVER_PIN_IS_OUTPUT

/**
 * @brief Helper Macro to check if the
 * GPIO is actual configrued as input
 * 
 */
#define GPIO_STATUS_IS_INPUT(status)           ((status & 0x30) == GPIO_DRIVER_PIN_IS_INPUT)
#define GPIO_STATUS_SET_INPUT(status)          status = (status & 0xCF) | GPIO_DRIVER_PIN_IS_INPUT

// --------------------------------------------------------------------------------

/**
 * @brief Helper Macro to check if the
 * GPIO is actual configrued as output
 * 
 */
#define GPIO_STATUS_IS_HIGH(status)             ((status & 0x30) == GPIO_DRIVER_PIN_IS_HIGH)
#define GPIO_STATUS_SET_HIGH(status)            status = (status & 0xCF) | GPIO_DRIVER_PIN_IS_HIGH

/**
 * @brief Helper Macro to check if the
 * GPIO is actual configrued as input
 * 
 */
#define GPIO_STATUS_IS_LOW(status)              ((status & 0x03) == GPIO_DRIVER_PIN_IS_LOW)
#define GPIO_STATUS_SET_LOW(status)             status = (status & 0xFC) | GPIO_DRIVER_PIN_IS_LOW

// --------------------------------------------------------------------------------

/**
 * @brief Definition of GPIO levels
 * 
 */
#define GPIO_LEVEL_HIGH                         1
#define GPIO_LEVEL_LOW                          0

// --------------------------------------------------------------------------------

/**
 * @brief Names of the gpios that are used to get the descriptors
 * 
 */
// static const char gpio_names[][] = {
//     "gpio00", "gpio01", "gpio02", "gpio03", "gpio04", "gpio05", "gpio06", "gpio07",
//     "gpio08", "gpio09", "gpio10", "gpio11", "gpio12", "gpio13", "gpio14", "gpio15",
//     "gpio16", "gpio17", "gpio18", "gpio19", "gpio20", "gpio21", "gpio22", "gpio23",
//     "gpio24", "gpio25", "gpio26", "gpio27", "gpio28", "gpio29", "gpio30", "gpio31"
// }

// --------------------------------------------------------------------------------

/**
 * @brief instance specific data of this driver
 * 
 */
typedef struct GPIO_DRIVER_INSTANCE_DATA_STRUCT {

    /**
     * @brief every instance of the gpio-driver
     * can handle a various number of gpios
     * The gpio-pins are requested in the moment they are changed
     * The index of this array correspond to the gpioto use.
     * On driver_open all fields are set to GPIO_DRIVER_PIN_UNUSED
     * 
     */
    uint8_t gpio_array[GPIO_DRIVER_MAX_NUM_OF_GPIO_PINS];

    /**
     * @brief Array of gpio-descriptors that are used while the lifetime of the driver
     * (open -> close). The whole array is initialized with NULL on open
     * For every gpio that is read or writen the corresponding index is set.
     * On close all indexs that are not null will be released.
     * 
     */
    struct gpio_desc* desc_array[GPIO_DRIVER_MAX_NUM_OF_GPIO_PINS];

} GPIO_DRIVER_INSTANCE_DATA;

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
static dev_t dev_number;

/**
 * @brief 
 * 
 */
static struct cdev* driver_object;

/**
 * @brief 
 * 
 */
static struct class* driver_class;

/**
 * @brief 
 * 
 */
static struct device* driver_dev;

// --------------------------------------------------------------------------------

/**
 * @brief Get the linux gpio index object
 * 
 * @param gpio_driver_index the gpio-index used inside of the gpio-driver
 * @return corresponding index of the linux sub-system of gpio_driver_index
 */
static int get_linux_gpio_index(uint8_t gpio_driver_index) {
    switch (gpio_driver_index) {
        case 0 : // no break;
        case 1 : // no break;
        case 2 : // no break;
        case 3 : // no break;
        case 4 : // no break;
        case 5 : // no break;
        case 6 : // no break;
        case 7 : // no break;
        case 8 : // no break;
        case 9 : // no break;
        case 10 : // no break;
        case 11 : // no break;
        case 12 : // no break;
        case 13 : // no break;
        case 14 : // no break;
        case 15 : // no break;
        case 16 : // no break;
        case 17 : // no break;
        case 18 : // no break;
        case 19 : // no break;
        case 20 : // no break;
        case 21 : // no break;
        case 22 : // no break;
        case 23 : // no break;
        case 24 : // no break;
        case 25 : return ((int)gpio_driver_index) + 2;
        default: return GPIO_DRIVER_GPIO_NUM_INVALID;
    }
}

// --------------------------------------------------------------------------------

/**
 * @brief Opens a new isntance of the gpio-driver.
 * Initializes the isntance data for this new instance.
 * 
 * @param device_file 
 * @param instance 
 * @return 0 if everything was initializes successfull, otherwise negative error-code
 */
static int driver_open(struct inode* device_file, struct file* instance) {

    GPIO_DRIVER_INSTANCE_DATA* p_instance_data = (GPIO_DRIVER_INSTANCE_DATA*) kmalloc(sizeof(GPIO_DRIVER_INSTANCE_DATA), GFP_KERNEL);
    if (p_instance_data == NULL) {

        PRINT_MSG("OPEN - ALLOCATE MEMORY FAILED\n");
        return -ENOMEM;

    } else {

        instance->private_data = (void*) p_instance_data;
    }
    
    uint32_t index = 0;
    for ( ; index < GPIO_DRIVER_MAX_NUM_OF_GPIO_PINS; index += 1) {
        p_instance_data->gpio_array[index] = GPIO_DRIVER_PIN_UNUSED;
        p_instance_data->desc_array[index] = NULL;
    }

    PRINT_MSG("OPEN\n");

    // addr = ioremap(GPIO_PORT_ADDR, GPIO_PORT_RANGE);

    // unsigned char *mem_io_address = 0x20200034;
    // uint32_t level0 = readl(mem_io_address);
    
    // mem_io_address = 0x20200038;
    // uint32_t level1 = readl(mem_io_address);

    // PRINT_MSG("LEVEL0: 0x%08X | LEVEL1: 0x%08X", level0, level1);

    return 0;
}

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param device_file 
 * @param isntance 
 * @return int 
 */
static int driver_close(struct inode* device_file, struct file* instance) {

    PRINT_MSG("CLOSE\n");

    GPIO_DRIVER_INSTANCE_DATA* p_instance_data = (GPIO_DRIVER_INSTANCE_DATA*) instance->private_data;

    uint32_t index = 0;
    for ( ; index < GPIO_DRIVER_MAX_NUM_OF_GPIO_PINS; index += 1) {
        if (p_instance_data->desc_array[index] != NULL) {
            //gpiod_put(p_instance_data->desc_array[index]);
            gpio_free(get_linux_gpio_index(index));
            PRINT_MSG("CLOSE - GPIO:%02u (LINUX:%02d) - DISPOSED\n", index, get_linux_gpio_index(index));
        }
    }

    // gpio_free(gpio_number);
    kfree(instance->private_data);

    return 0;
}

// --------------------------------------------------------------------------------

/**
 * @brief Modifies the level and direction of a gpio-pin.
 * 
 * @param instance 
 * @param user_data pointer to a memory-area of the type of
 * GPIO_DRIVER_INSTANCE_DATA where the pin-number the new direction
 * and the new level are set
 * @param count size of the given user-data in number of bytes
 * @param offset 
 * @return 0 if direction and level hav been set successful, otherwise negative error-number 
 * @see typedef struct GPIO_DRIVER_RW_CMD_STRUCT
 */
static ssize_t driver_write(struct file* instance, const char __user* user_data, size_t count, loff_t* offset) {

    if (instance->private_data == NULL) {
        PRINT_MSG("WRITE - INSTANCE DATA IS INVALID\n");
        return -ENOMEM;
    }

    if (count != sizeof(GPIO_DRIVER_RW_CMD)) {
        PRINT_MSG("WRITE - INV DATA LEN:%u (EXP:%u)\n", count, sizeof(GPIO_DRIVER_RW_CMD));
        return -EINVAL;
    }

    if (user_data == NULL) {
        PRINT_MSG("WRITE - NULL-POINTER\n");
        return -EINVAL;
    }

    /**
     * @brief Get the write command from the user-data.
     * User-data must be a type of GPIO_DRIVER_RW_CMD.
     * 
     */
    GPIO_DRIVER_RW_CMD write_cmd;
    if (copy_from_user(&write_cmd, user_data, sizeof(GPIO_DRIVER_RW_CMD)) != 0) {
        PRINT_MSG("WRITE - GET USER-DATA FAILED - BYTES LEFT\n");
        return -EAGAIN;
    }

    /**
     * @brief The mapped gpio number to use for operation
     * 
     */
    int linux_gpio_number = get_linux_gpio_index(write_cmd.gpio_number);

    if (linux_gpio_number == GPIO_DRIVER_GPIO_NUM_INVALID) {
        PRINT_MSG("WRITE - INVALID GPIO-NUM: %u\n", write_cmd.gpio_number);
        return -EINVAL;
    }

    /**
     * @brief Contet of this instance of the gpio-driver
     * 
     */
    GPIO_DRIVER_INSTANCE_DATA* p_instance_data = (GPIO_DRIVER_INSTANCE_DATA*) instance->private_data;

    /**
     * @brief check the command for plausibility
     * A toggle-command can only be performed if the gpio was initialized before
     * 
     */
    if (p_instance_data->gpio_array[write_cmd.gpio_number] == GPIO_DRIVER_PIN_UNUSED) {

        if (write_cmd.gpio_direction == GPIO_DRIVER_DIRECTION_TOGGLE) {
            PRINT_MSG("WRITE - GPIO:%02u (LINUX:%02d) - FAILED TOGGLE DIRECTION - UNINITIALIZED\n", write_cmd.gpio_number);
            return -EINVAL;
        }

        if (write_cmd.gpio_level == GPIO_DRIVER_LEVEL_TOGGLE) {
            PRINT_MSG("WRITE - GPIO:%02u (LINUX:%02d) - FAIELD TOGGLE LEVEL- UNINITIALIZED\n", write_cmd.gpio_number);
            return -EINVAL;
        }
    }

    if (p_instance_data->desc_array[write_cmd.gpio_number] == NULL) {

        PRINT_MSG("WRITE - GPIO:%02u (LINUX:%02d) - GET DESCRIPTOR\n", write_cmd.gpio_number, linux_gpio_number);

        p_instance_data->desc_array[write_cmd.gpio_number] = gpio_to_desc( linux_gpio_number );
        
        // gpiod_get_index(
        //     driver_dev,
        //     NULL/*gpio_names[write_cmd.gpio_number]*/,
        //     linux_gpio_number,
        //     GPIOD_ASIS
        // );
    }

    struct gpio_desc* p_gpio_descriptor = p_instance_data->desc_array[write_cmd.gpio_number];

    /**
     * @brief Optimization
     * Before calling system function to change direction and/or level,
     * check if direction and/or level must be changed
     */

    int return_value = 0;

    if (write_cmd.gpio_direction == GPIO_DRIVER_DIRECTION_TOGGLE) {
        if (GPIO_STATUS_IS_OUTPUT(p_instance_data->gpio_array[write_cmd.gpio_number])) {
            write_cmd.gpio_direction = GPIO_DRIVER_DIRECTION_INPUT;
        } else {
            write_cmd.gpio_direction = GPIO_DRIVER_DIRECTION_OUTPUT;
        }
    }

    if (write_cmd.gpio_direction == GPIO_DRIVER_DIRECTION_OUTPUT) {

        // return_value = gpio_direction_output(write_cmd.gpio_number);
        int level = (GPIO_STATUS_IS_HIGH(p_instance_data->gpio_array[write_cmd.gpio_number])) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;

        if (write_cmd.gpio_level == GPIO_DRIVER_LEVEL_HIGH) {
            PRINT_MSG("WRITE - OUTPUT - HIGH-LEVEL\n");
            GPIO_STATUS_SET_HIGH(p_instance_data->gpio_array[write_cmd.gpio_number]);
            level = GPIO_LEVEL_HIGH;

        } else if (write_cmd.gpio_level == GPIO_DRIVER_LEVEL_LOW) {
            PRINT_MSG("WRITE - OUTPUT - LOW-LEVEL\n");
            GPIO_STATUS_SET_LOW(p_instance_data->gpio_array[write_cmd.gpio_number]);
            level = GPIO_LEVEL_LOW;

        } else {
            PRINT_MSG("WRITE - OUTPUT - KEEP LEVEL:%d\n", level);
        }

        return_value = gpiod_direction_output(p_gpio_descriptor, level);
        GPIO_STATUS_SET_OUTPUT(p_instance_data->gpio_array[write_cmd.gpio_number]);
        PRINT_MSG("WRITE - GPIO:%02u (LINUX:%02d) - SET OUTPUT - LEVEL:%d\n", write_cmd.gpio_number, linux_gpio_number, level);

    } else if (write_cmd.gpio_direction == GPIO_DRIVER_DIRECTION_INPUT) {

        // return_value = gpio_direction_input(write_cmd.gpio_number);
        return_value = gpiod_direction_input(p_gpio_descriptor);
        GPIO_STATUS_SET_INPUT(p_instance_data->gpio_array[write_cmd.gpio_number]);
        PRINT_MSG("WRITE - GPIO:%02u (LINUX:%02d) - SET INPUT\n", write_cmd.gpio_number, linux_gpio_number);

    } else {

        // keep actual direction unchanged only set new level

        if (write_cmd.gpio_level == GPIO_DRIVER_LEVEL_TOGGLE) {

            if (GPIO_STATUS_IS_LOW(p_instance_data->gpio_array[write_cmd.gpio_number])) {

                PRINT_MSG("WRITE - GPIO:%02u (LINUX:%02d) - TOGGLE LEVEL - HIGH\n", write_cmd.gpio_number, linux_gpio_number);
                write_cmd.gpio_level = GPIO_DRIVER_LEVEL_HIGH;

            } else {

                PRINT_MSG("WRITE - GPIO:%02u (LINUX:%02d) - TOGGLE LEVEL - LOW\n", write_cmd.gpio_number, linux_gpio_number);
                write_cmd.gpio_level = GPIO_DRIVER_LEVEL_LOW;
            }

        } else if (write_cmd.gpio_level == GPIO_DRIVER_LEVEL_HIGH) {

            //gpio_set_value(get_linux_gpio_number(write_cmd.gpio_number),1);
            gpiod_set_value(p_gpio_descriptor, 1);
            GPIO_STATUS_SET_HIGH(p_instance_data->gpio_array[write_cmd.gpio_number]);
            PRINT_MSG("WRITE - GPIO:%02u (LINUX:%02d) - SET HIGH-LEVEL\n", write_cmd.gpio_number, linux_gpio_number);

        } else if (write_cmd.gpio_level == GPIO_DRIVER_LEVEL_LOW) {

            // gpio_set_value(get_linux_gpio_number(write_cmd.gpio_number),0);
            gpiod_set_value(p_gpio_descriptor, 0);
            GPIO_STATUS_SET_LOW(p_instance_data->gpio_array[write_cmd.gpio_number]);
            PRINT_MSG("WRITE - GPIO:%02u (LINUX:%02d) - SET LOW-LEVEL\n", write_cmd.gpio_number, linux_gpio_number);

        } else {
            PRINT_MSG("WRITE - GPIO:%02u (LINUX:%02d) - KEEP EVERYTHING AS IT ISn", write_cmd.gpio_number, linux_gpio_number);
        }
    }

    if (return_value != 0) {
        // gpio_free(write_cmd.gpio_number);
        PRINT_MSG("WRITE - GPIO:%02u (LINUX:%02d) - FAILED - ERROR: %d\n", write_cmd.gpio_direction, write_cmd.gpio_number, return_value);
        p_instance_data->gpio_array[write_cmd.gpio_number] = GPIO_DRIVER_PIN_UNUSED;
        return -1;
    }

    return 0;
}

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param instance 
 * @param buffer 
 * @param max_bytes_to_read 
 * @param offset 
 * @return ssize_t 
 */
static ssize_t driver_read(struct file* instance, char __user* user_data, size_t max_bytes_to_read, loff_t* offset) {

    if (instance->private_data == NULL) {
        PRINT_MSG("READ - INSTANCE DATA IS INVALID\n");
        return -ENOMEM;
    }

    if (max_bytes_to_read != sizeof(GPIO_DRIVER_RW_CMD)) {
        PRINT_MSG("READ - INV DATA LEN:%u (EXP:%u)\n", max_bytes_to_read, sizeof(GPIO_DRIVER_RW_CMD));
        return -EINVAL;
    }

    if (user_data == NULL) {
        PRINT_MSG("READ - NULL-POINTER\n");
        return -EINVAL;
    }

    /**
     * @brief Get the read command from the user-data.
     * User-data must be a type of GPIO_DRIVER_RW_CMD.
     * 
     */
    GPIO_DRIVER_RW_CMD read_cmd;
    if (copy_from_user(&read_cmd, user_data, sizeof(GPIO_DRIVER_RW_CMD)) != 0) {
        PRINT_MSG("READ - GET USER-DATA FAILED - BYTES LEFT\n");
        return -EAGAIN;
    }

    /**
     * @brief The mapped gpio number to use for operation
     * 
     */
    int linux_gpio_number = get_linux_gpio_index(read_cmd.gpio_number);

    if (linux_gpio_number == GPIO_DRIVER_GPIO_NUM_INVALID) {
        PRINT_MSG("READ - INVALID GPIO-NUM: %u\n", read_cmd.gpio_number);
        return -EINVAL;
    }

    /**
     * @brief Contet of this instance of the gpio-driver
     * 
     */
    GPIO_DRIVER_INSTANCE_DATA* p_instance_data = (GPIO_DRIVER_INSTANCE_DATA*) instance->private_data;

    /**
     * @brief Get the context of the actual GPIO-num for
     * operation. This is a read command so we do not want
     * to change the actual settings of the GPIO
     * 
     */
    if (p_instance_data->desc_array[read_cmd.gpio_number] == NULL) {

        PRINT_MSG("READ - GET GPIO - GPIO-NUM:%u - LINUX-NUM:%d\n", read_cmd.gpio_number, linux_gpio_number);

        p_instance_data->desc_array[read_cmd.gpio_number] = gpio_to_desc( linux_gpio_number );
        
        // gpiod_get_index(
        //     driver_dev,
        //     NULL/*gpio_names[read_cmd.gpio_number]*/,
        //     linux_gpio_number,
        //     GPIOD_IN
        // );

        if (gpiod_direction_input(p_instance_data->desc_array[read_cmd.gpio_number]) != 0) {
            PRINT_MSG("READ - INIT DIRECTION FAILED - GPIO-NUM:%u - LINUX-NUM:%d\n", read_cmd.gpio_number, linux_gpio_number);
        }
    }

    struct gpio_desc* p_gpio_descriptor = p_instance_data->desc_array[read_cmd.gpio_number];

    /**
     * @brief Get the direction of the GPIO
     * 
     */
    int return_value = gpiod_get_direction(p_gpio_descriptor);
    if (return_value == 1) {

        read_cmd.gpio_direction = GPIO_DRIVER_DIRECTION_INPUT;
        GPIO_STATUS_SET_INPUT(p_instance_data->gpio_array[read_cmd.gpio_number]);

    } else if (return_value == 0 ) {

        read_cmd.gpio_direction =  GPIO_DRIVER_DIRECTION_OUTPUT;
        GPIO_STATUS_SET_OUTPUT(p_instance_data->gpio_array[read_cmd.gpio_number]);

    } else {

        PRINT_MSG("READ - GET DIRECTION FAILED - GPIO-NUM:%u - ERR:%d\n", read_cmd.gpio_number, return_value);
        return return_value;
    }

    /**
     * @brief Get the level of the GPIO
     * 
     */
    return_value = gpiod_get_value(p_gpio_descriptor);
    if (return_value == 1) {

        read_cmd.gpio_level = GPIO_DRIVER_LEVEL_HIGH;
        GPIO_STATUS_SET_HIGH(p_instance_data->gpio_array[read_cmd.gpio_number]);

    } else if (return_value == 0 ) {

        read_cmd.gpio_level = GPIO_DRIVER_LEVEL_LOW;
        GPIO_STATUS_SET_LOW(p_instance_data->gpio_array[read_cmd.gpio_number]);

    } else {

        PRINT_MSG("READ - GET LEVEL FAILED - GPIO-NUM:%u - ERR:%d\n", read_cmd.gpio_number, return_value);
        return return_value;
    }

    if (copy_to_user(user_data, &read_cmd, sizeof(GPIO_DRIVER_RW_CMD)) != 0) {
        PRINT_MSG("READ - SET USER-DATA FAILED\n");
        return -EAGAIN;
    }

    PRINT_MSG("READ - GPIO-NUM:%u - DIRECTION:%u - LEVEL:%u\n", read_cmd.gpio_number, read_cmd.gpio_direction, read_cmd.gpio_level);

    return 0;
}

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
static struct file_operations driver_fops = {
    .owner = THIS_MODULE,
    .write = driver_write,
    .read = driver_read,
    .open = driver_open,
    .release = driver_close
};

// --------------------------------------------------------------------------------

#ifdef CONFIG_PW

/* Powermanagement */

/**
 * @brief 
 * 
 * @param dev 
 * @param pm_state 
 * @return int 
 */
static int driver_suspent(struct device* dev, pm_message_t pm_state) {
    switch (pm_state.event) {
        case PM_EVENT_ON : break;
        case PM_EVENT_FREEZE : break;
        case PM_EVENT_SUSPEND: break;
        case PM_EVENT_HIBERNATE : break;
        default: break;
    }

    return 0;
}

/**
 * @brief 
 * 
 * @param dev 
 * @return int 
 */
static int driver_resume(struct device* dev) {
    return 0;
}

#endif // CONFIG_PW

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @return int 
 */
static int __init mod_init(void) {

    int return_value = alloc_chrdev_region(
        &dev_number,    //
        0,              //
        1,              //
        DRIVER_NAME     //
    );

    if (return_value < 0) {
        PRINT_MSG("INIT - alloc_chrdev_region() FAILED\n");
        return -EIO;
    }

    // Anmelde-Objekt reservieren
    driver_object = cdev_alloc();
    if (driver_object == NULL) {
        PRINT_MSG("INIT - cdev_alloc() FAILED\n");
        unregister_chrdev_region(dev_number, 1);
        return -EIO;
    }

    driver_object->owner = THIS_MODULE;
    driver_object->ops = &driver_fops;

    return_value = cdev_add(driver_object, dev_number, 1);

    if (return_value != 0) {
        PRINT_MSG("INIT - cdev_add() FAILED\n");
        kobject_put(&driver_object->kobj);
        unregister_chrdev_region(dev_number, 1);
        return -EIO;
    } 

    // Eintrag im Sysfs, damit UDEV den Geraetedateieintrag erzeugt
    driver_class = class_create(THIS_MODULE, DRIVER_NAME);
    if (IS_ERR(driver_class) != 0) {
        PRINT_MSG("INIT - class_create() FAILED\n");
        kobject_put(&driver_object->kobj);
        unregister_chrdev_region(dev_number, 1);
        return -EIO;
    }

    driver_dev = device_create(
        driver_class,
        NULL,
        dev_number,
        NULL,
        "%s",
        DRIVER_NAME
    );

    //if (check_mem_region(GPIO_PORT_ADDR, GPIO_PORT_RANGE) == 0) {
    //    PRINT_MSG("INIT - check_mem_region() FAILED\n");
    //}

    //if (request_mem_region(GPIO_PORT_ADDR, GPIO_PORT_RANGE, DRIVER_NAME) == NULL) {
    //    PRINT_MSG("INIT - request_mem_region() FAILED\n");
    //    return -EIO;
    //}
 
    return 0;
}

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
static void __exit mod_exit(void) {

    //release_mem_region(GPIO_PORT_ADDR, GPIO_PORT_RANGE);

    // Loeschen des Syfs-Eintrags und damit der Geraetedatei
    device_destroy(driver_class, dev_number);
    class_destroy(driver_class);

    // Abmelden des Treibers
    cdev_del(driver_object);
    unregister_chrdev_region(dev_number, 1);
}

// --------------------------------------------------------------------------------

/**
 * @brief Construct a new module init object
 * 
 */
module_init(mod_init);

/**
 * @brief Construct a new module exit object
 * 
 */
module_exit(mod_exit);
 
/**
 * @brief 
 * 
 */
MODULE_LICENSE("GPL");

// --------------------------------------------------------------------------------
