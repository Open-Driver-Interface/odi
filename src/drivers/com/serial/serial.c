#include "serial.h"
#include "../../../deps.h"

uint8_t initialized = 0;
volatile serial_device_t *interrupted_serial_device = 0;

#pragma GCC diagnostic ignored "-Wunused-parameter"

serial_device_t serial_devices[] = {
    {
        .port = COM1_PORT,
        .irq = IRQ_COM1,
        .handler = COM1_HANDLER,
        .read_subscribers = 0,
        .write_subscribers = 0,
        .valid = 0
    },
    {
        .port = COM2_PORT,
        .irq = IRQ_COM2,
        .handler = COM2_HANDLER,
        .read_subscribers = 0,
        .write_subscribers = 0,
        .valid = 0
    },
    {
        .port = COM3_PORT,
        .irq = IRQ_COM3,
        .handler = COM3_HANDLER,
        .read_subscribers = 0,
        .write_subscribers = 0,
        .valid = 0
    },
    {
        .port = COM4_PORT,
        .irq = IRQ_COM4,
        .handler = COM4_HANDLER,
        .read_subscribers = 0,
        .write_subscribers = 0,
        .valid = 0
    }
};

void SERIAL_OF_HANDLER(void *frame) {
    if (interrupted_serial_device) {
        serial_flush(interrupted_serial_device->port);
        serial_discard(interrupted_serial_device->port);
    } else {
        odi_dep_printf("[WARNING] Serial overflow interrupt, but can't find device to flush\n");
    }
}

__attribute__((interrupt)) void COM1_HANDLER(void *frame) {
   serial_device_t* device = &(serial_devices[0]);
   if (!device->valid) return;
   interrupted_serial_device = device;
   char c = read_serial(device->port);
   write_buf_in(device, c);
   odi_dep_pic_eoi(device->irq);
}

__attribute__((interrupt)) void COM2_HANDLER(void *frame) {
   serial_device_t* device = &(serial_devices[1]);
   if (!device->valid) return;
   interrupted_serial_device = device;
   char c = read_serial(device->port);
   write_buf_in(device, c);
   odi_dep_pic_eoi(device->irq);
}
__attribute__((interrupt)) void COM3_HANDLER(void *frame) {
   serial_device_t* device = &(serial_devices[2]);
   if (!device->valid) return;
   interrupted_serial_device = device;
   char c = read_serial(device->port);
   write_buf_in(device, c);
   odi_dep_pic_eoi(device->irq);
}

__attribute__((interrupt)) void COM4_HANDLER(void *frame) {
   serial_device_t* device = &(serial_devices[3]);
   if (!device->valid) return;
   interrupted_serial_device = device;
   char c = read_serial(device->port);
   write_buf_in(device, c);
   odi_dep_pic_eoi(device->irq);
}


int serial_config_set(int port, uint8_t flags, serial_config_t config) {
    uint8_t lcr = odi_dep_inb(port + UART_LCR);

    if (flags & SERIAL_CFG_DIVISOR) {
        // Invalid baud (does not divide 115200)
        if (SERIAL_BAUD_MAX % config.divisor) {
            return 1;
        }

        odi_dep_outb(port + UART_LCR, 0x80); // Enable DLAB
        // Set DLL & DLH
        odi_dep_outb(port + UART_DLL, config.divisor & 0xff);
        odi_dep_outb(port + UART_DLL, config.divisor >> 8);
        odi_dep_outb(port + UART_LCR, lcr); // Restore LCR register
    }
    if (flags & SERIAL_CFG_DATABITS) {
        if (config.databits > 0b11) {
            return 1;
        }

        lcr |= config.databits;
    }
    if (flags & SERIAL_CFG_STOPBITS) {
        if (config.stopbits > 1) {
            return 1;
        }

        lcr |= config.stopbits << 2;
    }
    if (flags & SERIAL_CFG_PARITY) {
        if (config.parity > 4) {
            return 1;
        }

        lcr |= config.parity << 3;
    }

    odi_dep_outb(port + UART_LCR, lcr);

    return 0;
}

int serial_config_get(int port, serial_config_t *config) {
    uint8_t lcr = odi_dep_inb(port + UART_LCR);
    config->databits = lcr & 0b11;
    config->stopbits = lcr & 1;
    config->parity = lcr & 0b111000;

    odi_dep_outb(port + UART_LCR, 0x80); // Enable DLAB
    config->divisor = (odi_dep_inb(port + UART_DLH) << 8) | odi_dep_inb(port + UART_DLL);
    odi_dep_outb(port + UART_LCR, lcr); // Restore LCR register

    return 0;
}

int serial_received(int port) {
    return odi_dep_inb(port + UART_LSR) & 1;
}

char read_serial(int port) {
    while (serial_received(port) == 0);

    return odi_dep_inb(port);
}

int is_transmit_empty(int port) {
    return odi_dep_inb(port + UART_LSR) | 0x20;
}

void write_serial(int port, char c) {
    while (is_transmit_empty(port) == 0);
    odi_dep_outb(port, c);
}

void add_subscriber(serial_device_t *device, uint8_t type, void *parent, void *handler) {
    serial_subscriber_t *subscriber;
    if (type == SERIAL_READ) {
        subscriber = device->read_subscribers;
    } else if (type == SERIAL_WRITE) {
        subscriber = device->write_subscribers;
    } else {
        return;
    }

    while (subscriber->next) {
        subscriber = subscriber->next;
    }

    subscriber->next = odi_dep_malloc(sizeof(serial_subscriber_t));
    subscriber->next->handler = handler;
    subscriber->next->parent = parent;
    subscriber->next->next = 0;
}

void remove_subscriber(serial_device_t *device, uint8_t type, void *parent, void *handler) {
    serial_subscriber_t *subscriber;
    if (type == SERIAL_READ) {
        subscriber = device->read_subscribers;
    } else if (type == SERIAL_WRITE) {
        subscriber = device->write_subscribers;
    } else {
        return;
    }

    while (subscriber->next) {
        if (subscriber->next->handler == handler && subscriber->next->parent == parent) {
            serial_subscriber_t *to_remove = subscriber->next;
            subscriber->next = subscriber->next->next;
            odi_dep_free(to_remove);
            return;
        }
        subscriber = subscriber->next;
    }
}

void run_subscribers(serial_device_t *device, uint8_t type, char c) {
    serial_subscriber_t *subscriber;
    if (type == SERIAL_READ) {
        subscriber = device->read_subscribers;
    } else if (type == SERIAL_WRITE) {
        subscriber = device->write_subscribers;
    } else {
        return;
    }

    while (subscriber->next) {
        subscriber = subscriber->next;
        subscriber->handler(subscriber->parent, c, device->port);
    }
}

void write_buf_in(serial_device_t *device, char c) {
    device->buf_in[device->buf_in_write++] = c;
    if (device->buf_in_write >= device ->buf_in_size) device->buf_in_write = 0;
    if (device->buf_in_write == device->buf_in_read) {
        odi_dep_raise_interrupt(SERIAL_OF_IRQ);
    }
    if (device->echo) write_serial(device->port, c);
    run_subscribers(device, SERIAL_READ, c);
}

char read_buf_in(serial_device_t *device) {
    if (device->buf_in_read == device->buf_in_write) return 0;
    char c = device->buf_in[device->buf_in_read++];
    if (device->buf_in_read >= device->buf_in_size) device->buf_in_read = 0;
    return c;
}

void write_buf_out(serial_device_t *device, char c) {
    device->buf_in[device->buf_out_write++] = c;
    if (device->buf_out_write >= device ->buf_out_size) device->buf_out_write = 0;
    if (device->buf_out_write == device->buf_out_read) {
        odi_dep_raise_interrupt(SERIAL_OF_IRQ);
    }
    if (device->echo) write_serial(device->port, c);
}

char read_buf_out(serial_device_t *device) {
    if (device->buf_out_read == device->buf_out_write) return 0;
    char c = device->buf_out[device->buf_out_read++];
    if (device->buf_out_read >= device->buf_out_size) device->buf_out_read = 0;
    run_subscribers(device, SERIAL_WRITE, c);
    return c;
}

serial_device_t *get_serial_device(int port) {
   if (!initialized) return 0;

   for (int i = 0; i < MAX_COM_DEVICES; i++) {
      if (serial_devices[i].valid && serial_devices[i].port == port) {
         return &(serial_devices[i]);
      }
   }
   return 0;
}

volatile serial_device_t *get_last_interrupted_serial() {
   return interrupted_serial_device;
}

int init_serial_comm(int port) {

   odi_dep_outb(port + 1, 0x00);    // Disable all interrupts
   odi_dep_outb(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   odi_dep_outb(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   odi_dep_outb(port + 1, 0x00);    //                  (hi byte)
   odi_dep_outb(port + 3, 0x03);    // 8 bits, no parity, one stop bit
   odi_dep_outb(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   odi_dep_outb(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   odi_dep_outb(port + 4, 0x1E);    // Set in loopback mode, test the serial chip
   odi_dep_outb(port + 0, 0xAB);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
 
   // Check if serial is faulty (i.e: not same byte as sent)
   if(odi_dep_inb(port + 0) != 0xAB) {
      odi_dep_printf("Serial port %x seems to be down\n", port);
      #ifndef _VBOX_COMPAT
      return 0;
      #endif
   }
 
   // If serial is not faulty set it in normal operation mode
   odi_dep_outb(port + 4, 0x0F);

   return 1;
}

void enable_serial_interrupt(serial_device_t *device) {
   if (device) {
      odi_dep_hook_interrupt(device->irq, device->handler, 0);
      odi_dep_outb(device->port + 1, 0x01);    // Enable all interrupts
   }
}
 
void disable_serial_interrupt(serial_device_t *device) {
   if (device) {
      odi_dep_unhook_interrupt(device->irq, 0);
      odi_dep_outb(device->port + 1, 0x00);    // Disable all interrupts
   }
}

void init_serial(int buf_in_size, int buf_out_size) {
    if (initialized) return;
    odi_dep_printf("### Initializing serial devices ### \n");
    
    for (int i = 0; i < MAX_COM_DEVICES; i++) {
        serial_device_t *device = &serial_devices[i];
        if (init_serial_comm(device->port)) {
            device->buf_in_size = buf_in_size;
            device->buf_out_size = buf_out_size;
            device->buf_in = (char *)odi_dep_malloc(buf_in_size);
            odi_dep_memset(device->buf_in, 0, buf_in_size);
            device->buf_out = (char *)odi_dep_malloc(buf_out_size);
            odi_dep_memset(device->buf_out, 0, buf_out_size);
            device->echo = 0;
            device->buf_in_read = 0;
            device->buf_in_write = 0;
            device->buf_out_read = 0;
            device->buf_out_write = 0;
            device->read_subscribers = odi_dep_malloc(sizeof(serial_subscriber_t));
            device->write_subscribers = odi_dep_malloc(sizeof(serial_subscriber_t));
            device->read_subscribers->next = 0;
            device->write_subscribers->next = 0;
            device->write_subscribers->handler = 0;
            device->write_subscribers->handler = 0;

            if (device->buf_in && device->buf_out) {
                enable_serial_interrupt(device);
                device->valid = 1;
                odi_dep_printf("Serial por %x initialized successfully\n", device->port);
                serial_discard(device->port);
            }
        }
    }
    odi_dep_hook_interrupt(SERIAL_OF_IRQ, SERIAL_OF_HANDLER, 1);
    initialized = 1;
}

void serial_discard(int port) {
    if (!initialized) return;
    serial_device_t *device = get_serial_device(port);
    if (!device) return;

    odi_dep_memset(device->buf_in, 0, device->buf_in_size);
    odi_dep_memset(device->buf_out, 0, device->buf_out_size);

    device->buf_in_read = device->buf_in_write;
    device->buf_out_read = device->buf_out_write;
}

void serial_echo_enable(int port) {
    if (!initialized) return;
    serial_device_t *device = get_serial_device(port);
    if (!device) return;
    device->echo = 1;
}

void serial_echo_disable(int port) {
    if (!initialized) return;
    serial_device_t *device = get_serial_device(port);
    if (!device) return;
    device->echo = 0;
}

void serial_read_event_add(int port, void *parent, void (*handler)(void *parent, char c, int port)) {
    if (!initialized) return;

    serial_device_t *device = get_serial_device(port);
    if (!device) return;

    add_subscriber(device, SERIAL_READ, parent, handler);
}

void serial_read_event_remove(int port, void *parent, void (*handler)(void *parent, char c, int port)) {
    if (!initialized) return;

    serial_device_t *device = get_serial_device(port);
    if (!device) return;

    remove_subscriber(device, SERIAL_READ, parent, handler);
}

void serial_write_event_add(int port, void *parent, void (*handler)(void *parent, char c, int port)) {
    if (!initialized) return;

    serial_device_t *device = get_serial_device(port);
    if (!device) return;

    add_subscriber(device, SERIAL_WRITE, parent, handler);
}

void serial_write_event_remove(int port, void *parent, void (*handler)(void *parent, char c, int port)) {
    if (!initialized) return;

    serial_device_t *device = get_serial_device(port);
    if (!device) return;

    remove_subscriber(device, SERIAL_WRITE, parent, handler);
}

void serial_flush(int port) {
    if (!initialized) return;

    serial_device_t *device = get_serial_device(port);

    char c = read_buf_out(device);
    while (c) {
        write_serial(port, c);
        c = read_buf_out(device);
    }
}