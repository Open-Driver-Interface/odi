#ifndef _ODI_DEVICE_H
#define _ODI_DEVICE_H

#include "types.h"

struct odi_device_info {
    u8 init;
    u32 major;
    u32 minor;
    void * control;
    void * control_ex;
    struct odi_device_info * next;
};

//Iterates through all devices from a specific list.
//Returns a pointer to the next device
//If we are at the end of the list, it returns 0
struct odi_device_info * odi_device_next(struct odi_device_info* head);

//Gets the number of devices with a specific major
//Returns the number of devices
//If the major doesn't exist, it returns 0
u32 odi_device_get_count(u32 major);

//Gets all devices with a specific major
//Returns a pointer to the first device
//If the major doesn't exist, it returns 0
struct odi_device_info * odi_device_getall(u32 major);

//Gets a device
//Returns a pointer to the device
//If the device doesn't exist, it returns 0
struct odi_device_info * odi_device_get(u32 major, u32 minor);

//Gets the major bitfield
//This is an array of 256 bytes
//A zero bit means that the major is free
//Any other value means the number of devices with that major
//Returns the number of active majors
u32 odi_device_get_majors(u8* bitfield);


//Signals that a device has been initialized
//This flag is optional.
void odi_device_inialize(u32 major);

//Signals that a device has been deinitialized
//This flag is optional.
void odi_device_deinialize(u32 major);

//Changes the major of a device
//Returns a pointer to the device
//If the device doesn't exist, it returns 0
struct odi_device_info * odi_device_change_major(u32 major, u32 minor, u32 new_major);

//Creates a device
//Doesn't check that control is valid
//Returns a pointer to the device
//If the major doesn't exist, it creates it
struct odi_device_info * odi_device_register(u32 major, void* control, void * control_ex);

//Gets extended control data
//Returns a pointer to the control data
void * odi_device_get_extended_control(u32 major);

//Deletes a device
//Returns 0 if the device was deleted
//Returns 1 if the device was not deleted
u8 odi_device_unregister(u32 major, u32 minor);

//Deletes all devices with a specific major
//Returns the number of devices deleted
//If the return value is 0, then no devices were deleted
//If the return value is -1, then the major was invalid
s64 odi_device_unregister_all(u32 major);

#endif