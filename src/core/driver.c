#include "types.h"
#include "driver.h"
#include "../config.h"

struct odi_driver_info odi_drivers[ODI_MAX_MAJORS] = {0};

struct odi_driver_info* odi_driver_register(u32 major, void * functions, u8 license, const char * vendor) {
    if (major >= ODI_MAX_MAJORS) return 0;
    if (odi_drivers[major].major != 0) return 0;
    odi_drivers[major].valid = 1;
    odi_drivers[major].major = major;
    odi_drivers[major].functions = functions;
    odi_drivers[major].license = license;

    u32 i = 0;
    while (vendor[i] != 0 && i < 24) {
        odi_drivers[major].vendor[i] = vendor[i];
        i++;
    }
    
    return &odi_drivers[major];
}

//Unregisters a device driver given a major number
//Returns 0 if the driver was unregistered
//Returns 1 if the driver was not unregistered
u8 odi_driver_unregister(u32 major) {
    if (major >= ODI_MAX_MAJORS) return 1;
    if (odi_drivers[major].major == 0) return 1;
    odi_drivers[major].valid = 0;
    odi_drivers[major].major = 0;
    odi_drivers[major].functions = 0;
    odi_drivers[major].license = 0;
    for (u32 i = 0; i < 24; i++) {
        odi_drivers[major].vendor[i] = 0;
    }
    return 0;
}

//Gets all drivers with a specific major
//Returns a pointer to the first driver
//If the major doesn't exist, it returns 0
struct odi_driver_info * odi_driver_get(u32 major) {
    if (major >= ODI_MAX_MAJORS) return 0;
    if (odi_drivers[major].major == 0) return 0;
    return &odi_drivers[major];
}

//Exported functions, abstraction layer resides here
void * odi_driver_init(u32 major, void* iobuff, void* control) {
    if (major >= ODI_MAX_MAJORS) return 0;
    if (odi_drivers[major].major == 0) return 0;
    struct odi_driver_functions * functions = (struct odi_driver_functions *)odi_drivers[major].functions;
    if (functions->init == 0) return 0;
    return functions->init(&odi_drivers[major], iobuff, control);
}

void * odi_driver_exit(u32 major, void* iobuff, void* control) {
    if (major >= ODI_MAX_MAJORS) return 0;
    if (odi_drivers[major].major == 0) return 0;
    struct odi_driver_functions * functions = (struct odi_driver_functions *)odi_drivers[major].functions;
    if (functions->exit == 0) return 0;
    return functions->exit(&odi_drivers[major], iobuff, control);
}

u64 odi_driver_read(u32 major, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    if (major >= ODI_MAX_MAJORS) return 0;
    if (odi_drivers[major].major == 0) return 0;
    struct odi_driver_functions * functions = (struct odi_driver_functions *)odi_drivers[major].functions;
    if (functions->read == 0) return 0;
    return functions->read(&odi_drivers[major], iobuff, control, read_size, read_offset);
}

u64 odi_driver_write(u32 major, void* iobuff, void* control, u64 write_size, u64 write_offset) {
    if (major >= ODI_MAX_MAJORS) return 0;
    if (odi_drivers[major].major == 0) return 0;
    struct odi_driver_functions * functions = (struct odi_driver_functions *)odi_drivers[major].functions;
    if (functions->write == 0) return 0;
    return functions->write(&odi_drivers[major], iobuff, control, write_size, write_offset);
}

void * odi_driver_ioctl(u32 major, void* iobuff, void* control, u64 operation) {
    if (major >= ODI_MAX_MAJORS) return 0;
    if (odi_drivers[major].major == 0) return 0;
    struct odi_driver_functions * functions = (struct odi_driver_functions *)odi_drivers[major].functions;
    if (functions->ioctl == 0) return 0;
    return functions->ioctl(&odi_drivers[major], iobuff, control, operation);
}