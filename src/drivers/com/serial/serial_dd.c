#include "serial_dd.h"
#include "serial.h"

#include "../../../core/debug.h"
#include "../../../core/driver.h"
#include "../../../core/device.h"

#include "../../../deps.h"

void * _serial_dd_init(void * self, void* iobuff, void* control) {
    odi_debug_append(ODI_DTAG_INFO, "SERIAL DRIVER INIT\n");
    return 0;
}
void * _serial_dd_exit (void * self, void* iobuff, void* control) {
    exit_device(0); //This is the real driver function, it is defined in basic.h
    return 0;
}
u64 _serial_dd_read (void * self, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    odi_debug_append(ODI_DTAG_INFO, "SERIAL DRIVER READ\n");
    read_device(0, 0, 0); //This is the real driver function, it is defined in basic.h
    return 0;
}
u64 _serial_dd_write (void * self, void* iobuff, void* control, u64 write_size, u64 write_offset) {
    odi_debug_append(ODI_DTAG_INFO, "SERIAL DRIVER WRITE\n");
    write_device(0, 0, 0, 0); //This is the real driver function, it is defined in basic.h
    return 0;
}

void *_serial_dd_ioctl(void *self, void *iobuff, void *control, u64 operation) {
    uint64_t port = *(uint64_t *)self;
    switch (operation) {
        case SERIAL_IOCTL_CFG_SET: {
            serial_ioctl_config_set_t *cmd = (serial_ioctl_config_set_t *)control;
            return (void *)serial_config_set(cmd->port, cmd->flags, cmd->config);
        }
        case SERIAL_IOCTL_CFG_GET: {
            serial_ioctl_config_get_t *cmd = (serial_ioctl_config_get_t *)control;
            return (void *)serial_config_get(cmd->port, &cmd->config);
        }
        case SERIAL_IOCTL_FLUSH: {
            serial_flush(port);
            return (void *)1;
        }
        case SERIAL_IOCTL_SUBSCRIBE_READ: {
            serial_ioctl_subscriber_t *subscriber = (serial_ioctl_subscriber_t *)control;
            serial_read_event_add(port, subscriber->parent, subscriber->handler);
            return (void *)1;
        }
        case SERIAL_IOCTL_SUBSCRIBE_WRITE: {
            serial_ioctl_subscriber_t *subscriber = (serial_ioctl_subscriber_t *)control;
            serial_write_event_add(port, subscriber->parent, subscriber->handler);
            return (void *)1;
        }
        case SERIAL_IOCTL_UNSUBSCRIBE_READ: {
            serial_ioctl_subscriber_t *subscriber = (serial_ioctl_subscriber_t *)control;
            serial_read_event_remove(port, subscriber->parent, subscriber->handler);
            return (void *)1;
        }
        case SERIAL_IOCTL_UNSUBSCRIBE_WRITE: {
            serial_ioctl_subscriber_t *subscriber = (serial_ioctl_subscriber_t *)control;
            serial_write_event_remove(port, subscriber->parent, subscriber->handler);
            return (void *)1;
        }
        case SERIAL_IOCTL_ECHO_ENABLE: {
            serial_echo_enable(port);
            return (void *)1;
        }
        case SERIAL_IOCTL_ECHO_DISABLE: {
            serial_echo_disable(port);
            return (void *)1;
        }
        case SERIAL_IOCTL_DISCARD: {
            serial_discard(port);
            return (void *)1;
        }
    }

    return (void *)0;
}

struct odi_driver_functions serial_ops = {
    .init = _serial_dd_init,
    .exit = _serial_dd_exit,
    .read = _serial_dd_read,
    .write = _serial_dd_write,
    .ioctl = _serial_dd_ioctl
};

void serial_dd_init(void) {
    struct odi_driver_info *driver = odi_driver_register(SERIAL_DD_MAJOR, (void*)&serial_ops, SERIAL_DD_LICENSE, SERIAL_DD_VENDOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "SERIAL DRIVER REGISTER FAILED\n");
        return;
    }
}

void serial_dd_exit(void) {

    struct odi_driver_info *driver = odi_driver_get(SERIAL_DD_MAJOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "SERIAL DRIVER UNREGISTER FAILED\n");
        return;
    }
}