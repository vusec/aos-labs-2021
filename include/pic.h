#pragma once

/* IBM PC/AT has two 8259A PICs (Programmable Interrupt Controllers). These are
 * set up in a master/slave configuration, more specifically in a cascade
 * configuration where the slave is connected to one of the interrupt lines of
 * the master chip.
 *
 * The PIC accepts several external IRQs (interrupt requests), translates them
 * to system interrupts and feeds them into to the processor in order.
 */

/* I/O base address for the master PIC. */
#define PIC1      0x20
#define PIC1_CMD  PIC1
#define PIC1_DATA (PIC1 + 1)

/* I/O base address for the slave PIC. */
#define PIC2      0xa0
#define PIC2_CMD  PIC2
#define PIC2_DATA (PIC2 + 1)

/* The end-of-interrupt command code. */
#define PIC_EOI   0x20

/* ICW4 required. */
#define PIC_ICW1_ICW4       0x01
/* Single mode (otherwise cascade mode) */
#define PIC_IC1_SINGLE      0x02
/* Call address interval 4 (otherwise 8) */
#define PIC_ICW1_INTERVAL4  0x04
/* Level-triggered mode (otherwise edge-triggered) */
#define PIC_ICW1_LEVEL      0x08
/* Initialization */
#define PIC_ICW1_INIT       0x10

/* 8086/88 (MCS-80/85) mode. */
#define PIC_ICW4_8086       0x01
/* Automatic EOI mode */
#define PIC_ICW4_AUTO       0x02
/* Buffered mode slave. */
#define PIC_ICW4_BUF_SLAVE  0x08
/* Buffered mode master. */
#define PIC_ICW4_BUF_MASTER 0x0c
/* Special fully nested mode. */
#define PIC_ICW4_SFNM       0x10

/* Read the IRQ Request Register (IRR) */
#define PIC_READ_IRR        0x0a
/* Read the In-Service Register (ISR) */
#define PIC_READ_ISR        0x0b

/* The number of IRQs. */
#define PIC_NIRQS           16

/* IRQ at which the slave is connected to the master. */
#define IRQ_SLAVE           2

