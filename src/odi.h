#ifndef _ODI_H
#define _ODI_H

#include "core/types.h"
#include "core/driver.h"
#include "core/device.h"
#include "core/autoconf.h"

typedef void* odi_device_t;

//Only in automatic mode
u32 odi_autoconf(void * rsdp);

//Both available in manual and automatic mode
void * odi_manual_device_register(u32 major, void* control, void* control_ex);
u8 odi_manual_device_unregister(u32 major, u32 minor);

//Operations
odi_device_t odi_open(const char* device_name);
u64 odi_read(odi_device_t device, u64 offset, u64 size, void * buffer);
u64 odi_write(odi_device_t device, u64 offset, u64 size, void * buffer);
u64 odi_ioctl(odi_device_t device, u64 operation, void * buffer);
u64 odi_close(odi_device_t device);

//Debug
void odi_debug_list_devices(void);
void odi_debug_list_drivers(void);

#endif