#ifndef _GT_UART_H
#define _GT_UART_H


#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "at32f403a_407.h"
#include "at32f403a_407_gpio.h"
#include "stream_buffer.h"

#define UART_RX_BUF_SIZE 256

struct serial_rx_fifo
{
    /* software fifo */
    uint8_t *buffer;

    uint16_t put_index, get_index;

    uint8_t is_full;
};


struct serial_configure
{
    uint32_t baud_rate;

    uint32_t data_bits               :4;
    uint32_t stop_bits               :2;
    uint32_t parity                  :2;
    uint32_t bufsz                   :16;
    uint32_t reserved                :8;
};

struct gt_uart
{
    usart_type *uart;
    crm_periph_clock_type uart_clk;
    crm_periph_clock_type tx_clk;
    gpio_type * tx_port;
    uint16_t tx_pin;
    crm_periph_clock_type rx_clk;
    gpio_type * rx_port;
    uint16_t rx_pin;
    crm_periph_clock_type rs485_clk;
    gpio_type * rs485_port;
    uint16_t rs485_pin;
    IRQn_Type irqn;
    char *name;
};

struct serial_device
{
    const struct uart_ops *ops;
    struct serial_configure   config;
    struct gt_uart uart;

    void *serial_rx;
    void *serial_tx;
    StreamBufferHandle_t rx_buffer;

    void (*notify)(struct serial_device* dev);
};

struct uart_ops
{
    int (*configure)(struct serial_device *serial, struct serial_configure *cfg);

    int (*putc)(struct serial_device *serial, char c);
    int (*getc)(struct serial_device *serial);
};



int gt_hw_usart_init(void);
struct serial_device *gt_serial_get(const char *uart_name);
int gt_serial_open(struct serial_device *serial);
void gt_serial_set_rx_ind(struct serial_device *serial, void (*notify)(struct serial_device* dev));
int gt_serial_close(struct serial_device *serial);
int gt_serial_send(struct serial_device *serial, const uint8_t* buf, size_t len);
int gt_serial_recv(struct serial_device *serial, uint8_t* buf, size_t len, uint32_t wait_ms);

#endif



