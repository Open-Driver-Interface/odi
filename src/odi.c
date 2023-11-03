#include "odi.h"
#include "deps.h"
#include "core/debug.h"
#include "config.h"

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
odi_device_t odi_open(const char* device_name){
    struct identifier identifier;
    if (convert_name(device_name, &identifier) == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "[%s] ODI READ FAILED, DEVICE NAME INVALID\n", device_name);
        return NULL;
    }

    struct odi_device_info* device_info = odi_device_get(identifier.major, identifier.minor);
    if (device_info == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "[%s] ODI READ FAILED, DEVICE NOT FOUND\n", device_name);
        return NULL;
    }

    return (odi_device_t)device_info;
}

u64 odi_read(odi_device_t device, u64 offset, u64 size, void * buffer) {
    odi_assert(device != NULL);

    struct odi_device_info* device_info = (struct odi_device_info*)device;

    return odi_driver_read(device_info->major, buffer, device_info->control, size, offset);
}

u64 odi_write(odi_device_t device, u64 offset, u64 size, void * buffer) {
    odi_assert(device != NULL);

    struct odi_device_info* device_info = (struct odi_device_info*)device;

    //TODO: Make this return the number of bytes written
    odi_driver_write(device_info->major, buffer, device_info->control, size, offset);

    return 1;
}

u64 odi_ioctl(odi_device_t device, u64 operation, void * buffer) {
    odi_assert(device != NULL);

    struct odi_device_info* device_info = (struct odi_device_info*)device;

    //TODO: Make this return correctly
    odi_driver_ioctl(device_info->major, buffer, device_info->control, operation);

    return 1;
}

u64 odi_close(odi_device_t device){
    // TODO

    return 1;
}

//Debug
void odi_list_devices(void) {
    odi_debug_append(ODI_DTAG_INFO, "ODI LIST DEVICES REQUEST\n");
    for (int i = 0; i < ODI_MAX_MAJORS; i++) {
        struct odi_device_info * device = odi_device_getall(i);
        if (device == 0) continue;
        
        while (device != 0) {
            char name[64] = {0};
            odi_dep_strncpy(name, MAJOR_PREFIXES[device->major], odi_dep_strlen(MAJOR_PREFIXES[device->major]));
            name[odi_dep_strlen(MAJOR_PREFIXES[device->major])] = '0' + device->minor;
            odi_debug_append(ODI_DTAG_INFO, "[DEVICE %s] MAJ: %d | MIN: %d | INIT: %d | CONTROL: %p | CONTROL_EX: %p | NEXT: %p] \n", name, device->major, device->minor, device->init, device->control, device->control_ex, device->next);
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