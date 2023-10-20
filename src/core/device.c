#include "../config.h"
#include "types.h"
#include "device.h"
#include "../deps.h"

struct odi_device_info * odi_devices[ODI_MAX_MAJORS] = {0};

//Private functions

struct odi_device_info * add(struct odi_device_info* head, struct odi_device_info* new) {
    if (head == 0) return new;
    head->next = new;
    return head;
}

struct odi_device_info * remove(struct odi_device_info* head, struct odi_device_info* item) {
    if (head == 0) return 0;
    if (head == item) return head->next;
    head->next = remove(head->next, item);
    return head;
}

//Careful, this function is thread-unsafe
u32 find_free_minor(struct odi_device_info* head) {
    if (head == 0) return 0;
    u32 minor = 0;
    struct odi_device_info * current = head;
    while (current != 0) {
        if (current->minor == minor) {
            minor++;
            current = head;
        } else {
            current = current->next;
        }
    }
    return minor;
}

//Public functions

//Iterates through all devices from a specific list.
//Returns a pointer to the next device
//If we are at the end of the list, it returns 0
struct odi_device_info * odi_device_next(struct odi_device_info* head) {
    if (head == 0) return 0;
    return head->next;
}

//Gets the number of devices with a specific major
//Returns the number of devices
//If the major doesn't exist, it returns 0
u32 odi_device_get_count(u32 major) {
    if (major >= ODI_MAX_MAJORS) return 0;
    u32 count = 0;
    struct odi_device_info * current = odi_devices[major];
    while (current != 0) {
        count++;
        current = current->next;
    }
    return count;
}

//Gets all devices with a specific major
//Returns a pointer to the first device
//If the major doesn't exist, it returns 0
struct odi_device_info * odi_device_getall(u32 major) {
    if (major >= ODI_MAX_MAJORS) return 0;
    return odi_devices[major];
}

//Gets a device
//Returns a pointer to the device
//If the device doesn't exist, it returns 0
struct odi_device_info * odi_device_get(u32 major, u32 minor) {
    if (major >= ODI_MAX_MAJORS) return 0;
    struct odi_device_info * current = odi_devices[major];
    while (current != 0) {
        if (current->minor == minor) return current;
        current = current->next;
    }
    return 0;
}

//Gets the major bitfield
//This is an array of 256 bytes
//A zero bit means that the major is free
//Any other value means the number of devices with that major
//Returns the number of active majors
u32 odi_device_get_majors(u8* bitfield) {
    u32 count = 0;
    for (u32 i = 0; i < ODI_MAX_MAJORS; i++) {
        if (odi_devices[i] != 0) {
            bitfield[i] = odi_device_get_count(i);
            count++;
        } else {
            bitfield[i] = 0;
        }
    }
    return count;
}


//Changes the major of a device
//Returns a pointer to the device
//If the device doesn't exist, it returns 0
struct odi_device_info * odi_device_change_major(u32 major, u32 minor, u32 new_major) {
    struct odi_device_info * device = odi_device_get(major, minor);
    if (device == 0) return 0;
    if (major == new_major) return device;

    device->major = new_major;
    device->minor = find_free_minor(odi_devices[new_major]);
    odi_devices[major] = remove(odi_devices[major], device);
    odi_devices[new_major] = add(odi_devices[new_major], device);
    return device;
}


//Creates a device
//Doesn't check that control is valid
//Returns a pointer to the device
//If the major doesn't exist, it creates it
struct odi_device_info * odi_device_register(u32 major, void* control) {
    if (major >= ODI_MAX_MAJORS) return 0;
    struct odi_device_info * new = odi_dep_malloc(sizeof(struct odi_device_info));
    if (new == 0) return 0;
    new->valid = 1;
    new->major = major;
    new->minor = find_free_minor(odi_devices[major]);
    new->control = control;
    new->next = 0;
    odi_devices[major] = add(odi_devices[major], new);
    return new;
}

//Deletes a device
//Returns 0 if the device was deleted
//Returns 1 if the device was not deleted
u8 odi_device_unregister(u32 major, u32 minor) {
    if (major >= ODI_MAX_MAJORS) return 1;
    struct odi_device_info * current = odi_devices[major];
    struct odi_device_info * previous = 0;
    while (current != 0) {
        if (current->minor == minor) {
            if (previous == 0) {
                odi_devices[major] = current->next;
            } else {
                previous->next = current->next;
            }
            odi_dep_free(current);
            return 0;
        }
        previous = current;
        current = current->next;
    }
    return 1;
}

//Deletes all devices with a specific major
//Returns the number of devices deleted
//If the return value is 0, then no devices were deleted
//If the return value is -1, then the major was invalid
s64 odi_device_unregister_all(u32 major) {
    if (major >= ODI_MAX_MAJORS) return -1;
    struct odi_device_info * current = odi_devices[major];
    struct odi_device_info * previous = 0;
    u32 count = 0;
    while (current != 0) {
        count++;
        previous = current;
        current = current->next;
        odi_dep_free(previous);
    }
    odi_devices[major] = 0;
    return count;
}
