#ifndef _ODI_H
#define _ODI_H

#include "core/types.h"
#include "core/driver.h"
#include "core/device.h"
#include "core/autoconf.h"

//Only in automatic mode
u32 odi_autoconf(void * rsdp);

//Both available in manual and automatic mode
void * odi_manual_device_register(u32 major, void* control, void* control_ex);
u8 odi_manual_device_unregister(u32 major, u32 minor);

//Operations
u64 odi_read(const char * device, u64 offset, u64 size, void * buffer);
u64 odi_write(const char * device, u64 offset, u64 size, void * buffer);
u64 odi_ioctl(const char * device, u64 operation, void * buffer);

//Debug
void odi_list_devices(void);
void odi_list_drivers(void);

void odi_hello(void);
void odi_goodbye(void);
#endif