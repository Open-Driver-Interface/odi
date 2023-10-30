#ifndef _ODI_DRIVER_H
#define _ODI_DRIVER_H

#include "types.h"

#define ODI_LICENSE_TYPE_PUBLIC_DOMAIN  0
#define ODI_LICENSE_TYPE_GPLv1          1
#define ODI_LICENSE_TYPE_GPLv2          2
#define ODI_LICENSE_TYPE_GPLv3          3
#define ODI_LICENSE_TYPE_LGPLv1         4
#define ODI_LICENSE_TYPE_LGPLv2         5
#define ODI_LICENSE_TYPE_LGPLv3         6
#define ODI_LICENSE_TYPE_MIT            7
#define ODI_LICENSE_TYPE_BSD            8
#define ODI_LICENSE_TYPE_UNLICENSE      9
#define ODI_LICENSE_TYPE_UNCLASSIFIED   10
#define ODI_LICENSE_TYPE_PROPIETARY     255

struct odi_driver_functions {
    void * (*init) (void * self, void* iobuff, void* control);
    void * (*exit) (void * self, void* iobuff, void* control);
    u64 (*read) (void * self, void* iobuff, void* control, u64 read_size,  u64 read_offset);
    u64 (*write)(void * self, void* iobuff, void* control, u64 write_size, u64 write_offset);
    void * (*ioctl)(void * self, void* iobuff, void* control, u64 operation);
} __attribute__((packed));  

struct odi_driver_info {
    u8 valid;
    u32 major;
    struct odi_driver_functions * functions; //This can be any kind of driver function, usually generic_driver_functions
    u8 license;
    u8 vendor[24];
    struct odi_driver_info * next;
};

//Registers a new device driver given a major number, a pointer to a generic_driver_functions struct (or compatible)
//a license type and a vendor name (max 24 characters)
//Returns a pointer to the driver info struct
//If the major already exists, it returns 0
struct odi_driver_info* odi_driver_register(u32 major, void * functions, u8 license, const char * vendor);

//Unregisters a device driver given a major number
//Returns 0 if the driver was unregistered
//Returns 1 if the driver was not unregistered
u8 odi_driver_unregister(u32 major);

//Gets all drivers with a specific major
//Returns a pointer to the first driver
//If the major doesn't exist, it returns 0
struct odi_driver_info * odi_driver_get(u32 major);

//Exported functions, abstraction layer resides here
void * odi_driver_init(u32 major, void* iobuff, void* control);
void * odi_driver_exit(u32 major, void* iobuff, void* control);
u64 odi_driver_read(u32 major, void* iobuff, void* control, u64 read_size, u64 read_offset);
u64 odi_driver_write(u32 major, void* iobuff, void* control, u64 write_size, u64 write_offset);
void * odi_driver_ioctl(u32 major, void* iobuff, void* control, u64 operation);

#endif