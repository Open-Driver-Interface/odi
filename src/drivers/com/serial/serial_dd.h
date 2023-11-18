#ifndef _SERIAL_DD_H
#define _SERIAL_DD_H

#include "../../../core/types.h"

// IOCTL codes
#define SERIAL_IOCTL_CFG_SET            0x01
#define SERIAL_IOCTL_CFG_GET            0x02
#define SERIAL_IOCTL_FLUSH              0x03
#define SERIAL_IOCTL_SUBSCRIBE_READ     0x04
#define SERIAL_IOCTL_SUBSCRIBE_WRITE    0x05
#define SERIAL_IOCTL_UNSUBSCRIBE_READ   0x06
#define SERIAL_IOCTL_UNSUBSCRIBE_WRITE  0x07
#define SERIAL_IOCTL_ECHO_ENABLE        0x08
#define SERIAL_IOCTL_ECHO_DISABLE       0x09
#define SERIAL_IOCTL_DISCARD            0x0a

// This is required, export the driver name, major number
// and license type. This is used by the driver manager.
#define SERIAL_DD_LICENSE             ODI_LICENSE_TYPE_MIT
#define SERIAL_DD_VENDOR              "OPENDRIVERINTERFACE\0"
#define SERIAL_DD_NAME                "SERIAL DRIVER\0"
#define SERIAL_DD_MAJOR               123

/* IOCTL control structures */

typedef struct {
    int port;
    u8 flags;
    serial_config_t config;
} serial_ioctl_config_set_t;

typedef struct {
    int port;
    serial_config_t config;
} serial_ioctl_config_get_t;

typedef struct {
    void *parent;
    void (*handler)(void *parent, char c, int port);
} __attribute__((packed)) serial_ioctl_subscriber_t;

#endif