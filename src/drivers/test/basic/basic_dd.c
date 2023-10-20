#include "basic_dd.h"

//It is very important to include the real driver.
#include "basic.h"

//This include is just to print debug messages to the driver stack
#include "../../../core/debug.h"

//This is here to get the driver functions struct and the driver register functions
#include "../../../core/driver.h"

//Optional
#pragma GCC diagnostic ignored "-Wunused-parameter"

//Implementation of the driver functions, they can be empty if not used
void * basic_init(struct odi_driver_info * self, void* iobuff) {
    odi_debug_append(ODI_DTAG_INFO, "BASIC DRIVER INIT\n");
    init_device(0); //This is the real driver function, it is defined in basic.h
    return 0;
}
void * basic_exit (struct odi_driver_info * self, void* iobuff) {
    odi_debug_append(ODI_DTAG_INFO, "BASIC DRIVER EXIT\n");
    exit_device(0); //This is the real driver function, it is defined in basic.h
    return 0;
}
void * basic_read (struct odi_driver_info * self, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    odi_debug_append(ODI_DTAG_INFO, "BASIC DRIVER READ\n");
    read_device(0, 0, 0); //This is the real driver function, it is defined in basic.h
    return 0;
}
void * basic_write (struct odi_driver_info * self, void* iobuff, void* control, u64 write_size, u64 write_offset) {
    odi_debug_append(ODI_DTAG_INFO, "BASIC DRIVER WRITE\n");
    write_device(0, 0, 0, 0); //This is the real driver function, it is defined in basic.h
    return 0;
}
void * basic_ioctl (struct odi_driver_info * self, void* iobuff, void* control, u64 operation) {
    odi_debug_append(ODI_DTAG_INFO, "BASIC DRIVER IOCTL\n");
    
    //We call the specific operation with this switch
    switch (operation) {
        case IOCTL_BASIC_SAY_HI:
            odi_debug_append(ODI_DTAG_INFO, "BASIC DRIVER IOCTL SAY HI\n");
            do_cool_things(); //This is the real driver function, it is defined in basic.h
            break;
        case IOCTL_BASIC_BE_BAD:
            odi_debug_append(ODI_DTAG_INFO, "BASIC DRIVER IOCTL BE BAD\n");
            do_dumb_things(); //This is the real driver function, it is defined in basic.h
            break;
        default:
            odi_debug_append(ODI_DTAG_INFO, "BASIC DRIVER IOCTL UNKNOWN\n");
            break;
    }

    return 0;
}

//This is where the driver is defined, it is a bad idea to set functions to null, it is
//better to set them to empty functions. ¡Don't set init and exit to null!
struct odi_driver_functions basic_ops = {
    .init = basic_init,
    .exit = basic_exit,
    .read = basic_read,
    .write = basic_write,
    .ioctl = basic_ioctl
};

//Life cycle functions, they are called by the kernel in manual mode or by the driver in managed mode.
void init_basic(void) {
    odi_debug_append(ODI_DTAG_INFO, "BASIC DRIVER REGISTER STARTED\n");
    struct odi_driver_info * driver = odi_driver_register(BASIC_DD_MAJOR, (void*)&basic_ops, BASIC_DD_LICENSE, BASIC_DD_VENDOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "BASIC DRIVER REGISTER FAILED\n");
        return;
    }

    ((struct odi_driver_functions*)driver->functions)->init(driver, 0);

    odi_debug_append(ODI_DTAG_INFO, "BASIC DRIVER REGISTER FINISHED\n");
}

void exit_basic(void) {
    odi_debug_append(ODI_DTAG_INFO, "BASIC DRIVER UNREGISTER STARTED\n");
    struct odi_driver_info * driver = odi_driver_get(BASIC_DD_MAJOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "BASIC DRIVER UNREGISTER FAILED\n");
        return;
    }

    ((struct odi_driver_functions*)driver->functions)->exit(driver, 0);

    if (odi_driver_unregister(BASIC_DD_MAJOR)) {
        odi_debug_append(ODI_DTAG_ERROR, "BASIC DRIVER UNREGISTER FAILED\n");
        return;
    }

    odi_debug_append(ODI_DTAG_INFO, "BASIC DRIVER UNREGISTER FINISHED\n");
}