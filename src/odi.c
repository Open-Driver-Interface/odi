#include "odi.h"
#include "deps.h"
#include "core/debug.h"
#include "config.h"

//disable -Woverride-init
#pragma GCC diagnostic ignored "-Woverride-init"

const char* MAJOR_PREFIXES[ODI_MAX_MAJORS] = {
    [0x0 ... ODI_MAX_MAJORS -1] = "none",
    [8] = "hd",   // 8
    [9] = "cd",   // 9
    [0xa] = "semb", // a
    [0xb] = "pm",   // b
    [0xc] = "umsd", // c
    [0xd] = "usb", // d
    [0xe] = "tty", // e
    [0xf] = "kbd", // f
    [0x10] = "mouse", // 10
    [0x11] = "net", // 11
    [86] = "pci",
    [255] = "basic"
};

struct identifier {
    u32 major;
    u32 minor;
};

u8 convert_name(const char * name , struct identifier * identifier) {
    //Get last character of name
    char last = name[odi_dep_strlen(name) - 1];
    //The minor is the last character converted to a number
    //0 is equivalent to a and A.
    if (last >= '0' && last <= '9') {
        identifier->minor = last - '0';
    } else if (last >= 'a' && last <= 'z') {
        identifier->minor = last - 'a';
    } else if (last >= 'A' && last <= 'Z') {
        identifier->minor = last - 'A';
    } else {
        return 0;
    }

    //The major is the rest of the name as an index into MAJOR_PREFIXES
    u32 length = odi_dep_strlen(name);
    char major[length];
    odi_dep_strncpy(major, name, length - 1);

    for (u32 i = 0; i < ODI_MAX_MAJORS; i++) {
        if (odi_dep_strcmp(major, MAJOR_PREFIXES[i]) == 0) {
            identifier->major = i;
            return 1;
        }
    }

    return 0;
}

//Only in automatic mode
u32 odi_autoconf(void * rsdp) {
    odi_autoconf_scan(rsdp);
    return 0;
}

//Both available in manual and automatic mode
void * odi_manual_device_register(u32 major, void* control, void* control_ex) {
    return odi_device_register(major, control, control_ex);
}

u8 odi_manual_device_unregister(u32 major, u32 minor) {
    return odi_device_unregister(major, minor);
}

//Operations
u64 odi_read(const char * device, u64 offset, u64 size, void * buffer) {
    struct identifier identifier;
    if (convert_name(device, &identifier) == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "[%s] ODI READ FAILED, DEVICE NAME INVALID\n", device);
        return 0;
    }

    struct odi_device_info * device_info = odi_device_get(identifier.major, identifier.minor);
    if (device_info == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "[%s] ODI READ FAILED, DEVICE NOT FOUND\n", device);
        return 0;
    }

    struct odi_driver_info * driver_info = odi_driver_get(identifier.major);
    if (driver_info == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "[%s] ODI READ FAILED, DRVIER NOT FOUND\n", device);
        return 0;
    }

    //TODO: Make this return the number of bytes written
    odi_driver_read(device_info->major, buffer, device_info->control, size, offset);

    return 1;
}

u64 odi_write(const char * device, u64 offset, u64 size, void * buffer) {
    struct identifier identifier;
    if (convert_name(device, &identifier) == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "[%s] ODI WRITE FAILED, DEVICE NAME INVALID\n", device);
        return 0;
    }

    struct odi_device_info * device_info = odi_device_get(identifier.major, identifier.minor);
    if (device_info == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "[%s] ODI WRITE FAILED, DEVICE NOT FOUND\n", device);
        return 0;
    }

    struct odi_driver_info * driver_info = odi_driver_get(identifier.major);
    if (driver_info == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "[%s] ODI WRITE FAILED, DRVIER NOT FOUND\n", device);
        return 0;
    }

    //TODO: Make this return the number of bytes written
    odi_driver_write(device_info->major, buffer, device_info->control, size, offset);
    return 1;
}

u64 odi_ioctl(const char * device, u64 operation, void * buffer) {
    struct identifier identifier;
    if (convert_name(device, &identifier) == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "[%s] ODI IOCTL FAILED, DEVICE NAME INVALID\n", device);
        return 0;
    }

    struct odi_device_info * device_info = odi_device_get(identifier.major, identifier.minor);
    if (device_info == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "[%s] ODI IOCTL FAILED, DEVICE NOT FOUND\n", device);
        return 0;
    }

    struct odi_driver_info * driver_info = odi_driver_get(identifier.major);
    if (driver_info == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "[%s] ODI IOCTL FAILED, DRVIER NOT FOUND\n", device);
        return 0;
    }

    //TODO: Make this return the number of bytes written
    odi_driver_ioctl(device_info->major, buffer, device_info->control, operation);

    return 1;
}

//Debug
void odi_list_devices() {
    odi_debug_append(ODI_DTAG_INFO, "ODI LIST DEVICES REQUEST\n");
    for (int i = 0; i < ODI_MAX_MAJORS; i++) {
        struct odi_device_info * device = odi_device_getall(i);
        if (device == 0) continue;
        odi_debug_append(ODI_DTAG_INFO, "[DUMPING MAJOR %d]\n", i);
        
        while (device != 0) {
            odi_debug_append(ODI_DTAG_INFO, "[DEVICE] MAJ: %d | MIN: %d | INIT: %d | CONTROL: %p | CONTROL_EX: %p | NEXT: %p] \n", device->major, device->minor, device->init, device->control, device->control_ex, device->next);
            device = odi_device_next(device);
        }
    }
    odi_debug_flush(ODI_DTAG_INFO);
}

void odi_list_drivers(void) {
    odi_debug_append(ODI_DTAG_INFO, "ODI LIST DRIVERS REQUEST\n");
    for (int i = 0; i < ODI_MAX_MAJORS; i++) {
        struct odi_driver_info * driver = odi_driver_get(i);
        if (driver == 0) continue;
        odi_debug_append(ODI_DTAG_INFO, "[DRIVER] MAJ: %d | VALID: %d |  FUNCTIONS: %p | LICENSE: %d | VENDOR: %s | NEXT: %p] \n", driver->major, driver->valid, driver->functions, driver->license, driver->vendor, driver->next);
    }
    odi_debug_flush(ODI_DTAG_INFO);
}

void odi_hello(void) {
    odi_debug_append(ODI_DTAG_INFO, "Hello from ODI!\n");
    odi_debug_flush(ODI_DTAG_INFO);
}