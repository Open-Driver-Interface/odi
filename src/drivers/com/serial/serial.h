#ifndef _SERIAL_H
#define _SERIAL_H

#include <stdint.h>

/* UART Registers */

#define UART_RX  0 // Transmitter Holding Buffer (DLAB = 0)
#define UART_TX  0 // Receiver Buffer (DLAB = 0)
#define UART_DLL 0 // Divisor Latch Low Byte (DLAB = 1)
#define UART_IER 1 // Interrupt Enable Register (DLAB = 0)
#define UART_DLH 1 // Divisor Latch High Byte (DLAB = 1)
#define UART_IIR 2 // Interrupt Identification Register
#define UART_FCR 2 // FIFO Control Register
#define UART_LCR 3 // Line Control Register
#define UART_MCR 4 // Modem Control Register
#define UART_LSR 5 // Line Status Register
#define UART_MSR 6 // Modem Status Register
#define UART_SR  7 // Scratch Register

#define SERIAL_BAUD_MAX         115200

#define SERIAL_CFG_DIVISOR      (1 << 0)
#define SERIAL_CFG_DATABITS     (1 << 1)
#define SERIAL_CFG_STOPBITS     (1 << 2)
#define SERIAL_CFG_PARITY       (1 << 3)

#define SERIAL_115200_DIVISOR   1
#define SERIAL_57600_DIVISOR    2
#define SERIAL_38400_DIVISOR    3
#define SERIAL_19200_DIVISOR    6
#define SERIAL_9600_DIVISOR     12
#define SERIAL_4800_DIVISOR     24
#define SERIAL_2400_DIVISOR     48
#define SERIAL_1200_DIVISOR     96
#define SERIAL_600_DIVISOR      192
#define SERIAL_300_DIVISOR      384
#define SERIAL_220_DIVISOR      524
#define SERIAL_110_DIVISOR      1047
#define SERIAL_50_DIVISOR       2304

#define SERIAL_DATABITS_8   3
#define SERIAL_DATABITS_7   2
#define SERIAL_DATABITS_6   1
#define SERIAL_DATABITS_5   0

#define SERIAL_STOPBITS_1   0
#define SERIAL_STOPBITS_2   1
#define SERIAL_STOPBITS_1_5 1

#define SERIAL_PARITY_NONE  0
#define SERIAL_PARITY_ODD   1
#define SERIAL_PARITY_EVEN  2
#define SERIAL_PARITY_MARK  3
#define SERIAL_PARITY_SPACE 4

#define SERIAL_READ  1
#define SERIAL_WRITE 2

#define MAX_COM_DEVICES 2
#define SERIAL_OF_IRQ 0x91
#define COM1_PORT 0x3f8
#define COM2_PORT 0x2f8
#define COM3_PORT 0x3e8
#define COM4_PORT 0x2e8
#define IRQ_COM1 0x4
#define IRQ_COM2 0x3
#define IRQ_COM3 0x4
#define IRQ_COM4 0x3

typedef struct serial_subscriber {
    void *parent;
    void (*handler)(void *parent, char c, int port);
    struct serial_subscriber *next;
} serial_subscriber_t;

typedef struct {
    uint8_t valid;
    int port;
    int irq;
    uint8_t echo;

    void (*handler)(void *parent, char c, int port);
    serial_subscriber_t *read_subscribers;
    serial_subscriber_t *write_subscribers;

    char *buf_in;
    int buf_in_size;
    int buf_in_write;
    int buf_in_read;

    char *buf_out;
    int buf_out_size;
    int buf_out_write;
    int buf_out_read;

} serial_device_t;

/*
+---------------------------------------------------------------+
|                  LCR (Line Control Register)                  |
+---------------------------------------------------------------+
|   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
+-------+-------+-------+-------+-------+-------+-------+-------|
| DLAB  |  BRK  |        PARITY         | STOP  |     DATA      |
+-------+-------+-------+-------+-------+-------+-------+-------|
*/
typedef struct {
    uint16_t    divisor;    // Controls the baud rate
    uint8_t     databits;   // 5,6,7 or 8
    uint8_t     stopbits;   // 1 or 2
    uint8_t     parity;     // NONE, ODD, EVEN, MARK or SPACE
    uint8_t     fifo;
    uint8_t     fifo_threshold;
} serial_config_t;

int serial_config_set(int port, uint8_t flags, serial_config_t config);
int serial_config_get(int port, serial_config_t *config);

int serial_received(int port);
char read_serial(int port);

int is_transmit_empty(int port);
void write_serial(int port, char c);

void add_subscriber(serial_device_t *device, uint8_t type, void *parent, void *handler);
void remove_subscriber(serial_device_t *device, uint8_t type, void *parent, void *handler);
void run_subscribers(serial_device_t *device, uint8_t type, char c);

serial_device_t *get_serial_device(int port);
volatile serial_device_t *get_last_interrupted_serial();

int init_serial_comm(int port);

void enable_serial_interrupt(serial_device_t *device);
void disable_serial_interrupt(serial_device_t *device);

void init_serial(int buf_in_size, int buf_out_size);

void serial_flush(int port);
void serial_discard(int port);

void serial_echo_enable(int port);
void serial_echo_disable(int port);

// Subscriptions
void write_buf_in(serial_device_t *device, char c);
char read_buf_in(serial_device_t *device);
void write_buf_out(serial_device_t *device, char c);
char read_buf_out(serial_device_t *device);

void serial_read_event_add(int port, void *parent, void (*handler)(void *parent, char c, int port));
void serial_read_event_remove(int port, void *parent, void (*handler)(void *parent, char c, int port));
void serial_write_event_add(int port, void *parent, void (*handler)(void *parent, char c, int port));
void serial_write_event_remove(int port, void *parent, void (*handler)(void *parent, char c, int port));

#endif