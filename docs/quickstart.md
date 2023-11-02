# Quickstart Guide

To get started with OpenDriverInterface, follow these steps:

## Installation

1. First, make sure you have all the dependencies installed as specified in [dependencies.md](dependencies.md).

## Running the Example

1. Clone this repository and navigate to the `testenv` directory.

    ```bash
    git clone https://github.com/your-repo.git
    cd your-repo/testenv
    ```

2. If you don't have your own OS :
    21. Setup the environment using `make`:

        ```bash
        make setup
        ```

    22. Run the software:

        ```bash
        make
        ```

3. If you want to port ODI to your OS :
    31. Get one c file to build ODI:
        
        ```sh
        ofc.sh
        ```
        
    32. Port functions listed in `deps.h`

## Implementing the Library

If you want to integrate our library into your own operating system or project, you can do so by following these steps:

1. Copy the contents of the `source` folder to your project.

2. Implement the necessary functions in `dependencies.c` according to your operating system's requirements.

3. Include the `odi.h` header (and any other that you may need) in your source files and use the library as described in the [usage](#usage) section.

## Usage

### Fully Managed (PCI Devices Only)

If you want to use the fully managed mode for PCI devices, you should provide the RSDT (Root System Description Table) pointer to the `odi_start_managed` function. Here's how you can use it:

```c
// Include the necessary headers
#include "odi.h"

// Provide the RSDT pointer
void* rsdt_pointer = /* your RSDT pointer */;

// Start in fully managed mode
odi_init_managed(rsdt_pointer);

// Get the number of devices
uint32_t device_count = odi_device_get_count();

// Create a buffer to store the device information
struct odi_device_info* devices = malloc(sizeof(struct odi_device_info) * device_count);

// Get the device information
if (!odi_device_getall(devices, device_count)) {
    // Handle error
}

uint32_t device_id = /* your device ID */;

// Get a specific device by its ID (you can also access the devices array directly)
struct odi_device_info* device = odi_device_get(device_id);

// Get the driver associated with the device
struct odi_driver_info* driver = odi_driver_get(device);

// Call any function on the driver
driver->function(device, /* additional parameters */);

### How to get the RSDT pointer with limine

First of all, you may want to get the RSDT without limine, thats absolutely possible. [See how](https://wiki.osdev.org/RSDP#Detecting_the_RSDP)

Else, you should clone limine, copy the .EFI file to your image, create the limine.cfg file. An example [Here](https://github.com/limine-bootloader/limine-barebones)

You can check the limine boot protocol documentation [Here](https://github.com/limine-bootloader/limine/blob/v5.x-branch/PROTOCOL.md)

Then basically you have to copy the [limine.h](https://raw.githubusercontent.com/limine-bootloader/limine/v5.x-branch/limine.h) file to your source.

And request the rsdp with this code fragment:

```c
static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

uint64_t get_rsdp_address() {
    return (uint64_t)(rsdp_request.response)->address;
}
```

### Manual mode

In this mode you have to provide the device information manually. Each one is slightly different, so you should check the documentation for each one.

```c
struct odi_device_info {
    u8 valid;
    u32 major;
    u32 minor;
    void * control;
    struct odi_device_info * next;
};
```

There are a few fields that are always required, though:

- `u32 major`: The device driver identifier. This is used to identify the device type. See the [device types](#device-types) section for more information.
- `u32 minor`: The device identifier inside that specific type. See [device identification](#device-identification) for more information.	
- `void* control`: This field depends on the device type. See [device control](#device-control) for more information.


`u8 valid` and `struct odi_device_info * next` are set automatically once you register the device with the `odi_register_device` function.

### Driver registration

For rolling your own driver, fill the following structure:

```c
struct odi_driver_info {
    u8 valid;
    u32 major;
    void * functions; //This can be any kind of driver function, usually generic_driver_functions
    u8 license;
    u8 vendor[24];
};
```

- `u32 major`: The device driver identifier. This is used to identify the device type. See the [device types](#device-types) section for more information.
- `void* functions`: This field depends on the driver functions. See [driver functions](#driver-functions) for more information.
- `u8 license`: The license of the driver. See [licenses](#licenses) for more information.
- `u8 vendor[24]`: The vendor of the driver. In string of 24 characters.

`u8 valid` is set automatically once you register the driver with the `odi_register_driver` function.

### Driver functions

And implement as many functions as you wish in the functions struct. The default one is:

```c
struct odi_driver_functions {
    void * (*init) (struct odi_driver_info * self, void* iobuff);
    void * (*exit) (struct odi_driver_info * self, void* iobuff);
    void * (*read) (struct odi_driver_info * self, void* iobuff, u32 minor, u64 read_size, u64 read_offset);
    void * (*write)(struct odi_driver_info * self, void* iobuff, u32 minor, u64 write_size, u64 write_offset);
    void * (*ioctl)(struct odi_driver_info * self, void* iobuff, u32 minor, u64 operation);
} __attribute__((packed));

```

The parameters are:

- `init`: Called when the driver is loaded. You can use this to initialize the driver.
- `exit`: Called when the driver is unloaded. You can use this to free any resources.
- `read`: Called when the device is read. You can use this to read from the device. With u32 minor you can identify the device.
- `write`: Called when the device is written. You can use this to write to the device. With u32 minor you can identify the device.
- `ioctl`: Called when the device is controlled. You can use this to perform advanced operations. With u32 minor you can identify the device.

`iobuff` is a buffer that can be used as you wish. It is passed to the functions as a parameter, and it is not used by the library.

It is mandatory that you implement at least the above functions (you can leave all but init and exit as NULL).

If you use managed mode, you should encapsulate all advanced operations in the ioctl function, and use the read and write functions for basic operations.

### Device types

The device type is a 32-bit integer that identifies the driver that will handle this device. The following types are currently supported:

- `ODI_MAJOR_TTY`: tty device.
- `ODI_MAJOR_SERIAL`: serial device.
- `ODI_MAJOR_ATA`: ATA device.
- `ODI_MAJOR_ATAPI`: ATAPI device.
- `ODI_MAJOR_USB`: USB device.
- `ODI_MAJOR_NETWORK`: PCI network card device.
- `ODI_MAJOR_PS2_KEYBOARD`: PS2 Keyboard device.
- `ODI_MAJOR_PCI`: GenericPCI device.

### Device identification

The identification usually is made by a id string calculated from the major and minor numbers. 

For example, the major number for a tty device is `ODI_MAJOR_TTY` which has the assotiated string "tty". The minor number will be
converted to a letter and concatenated. So the device with major `ODI_MAJOR_TTY` and minor `0` will be identified as `ttya`.

You can get the id string with the `odi_get_device_id` function.

### Device control

The control field is a pointer to a structure that contains the device-specific information. For example, for a tty device, it will contain the tty buffer and the tty type. For a PCI device, it will contain the PCI device information. This will be used internally by the driver to perform the operations.


### Special cases

There are a few edge cases like network controllers where the r/w/ioctl scheme doesn't make sense, here we specify an alternative driver functions struct.

### Licenses

Supported licenses are:

- `ODI_LICENSE_TYPE_PUBLIC_DOMAIN`: Public domain.
- `ODI_LICENSE_TYPE_GPLv1`: GNU General Public License v1.
- `ODI_LICENSE_TYPE_GPLv2`: GNU General Public License v2.
- `ODI_LICENSE_TYPE_GPLv3`: GNU General Public License v3.
- `ODI_LICENSE_TYPE_LGPLv1`: GNU Lesser General Public License v1.
- `ODI_LICENSE_TYPE_LGPLv2`: GNU Lesser General Public License v2.
- `ODI_LICENSE_TYPE_LGPLv3`: GNU Lesser General Public License v3.
- `ODI_LICENSE_TYPE_MIT`: MIT License.
- `ODI_LICENSE_TYPE_BSD`: BSD License.
- `ODI_LICENSE_TYPE_UNLICENSE`: Unlicense.
- `ODI_LICENSE_TYPE_UNCLASSIFIED`: Unclassified.
- `ODI_LICENSE_TYPE_PROPIETARY`: Propietary.
