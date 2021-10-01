#pragma once

/* Available LAPIC registers. */
/* LAPIC ID */
#define LAPIC_ID        0x0020
/* LAPIC version */
#define LAPIC_VERSION   0x0030
/* Task Priority Register */
#define LAPIC_TPR       0x0080
/* End Of Interrupt */
#define LAPIC_EOI       0x00b0
/* Spurious Interrupt Vector Register */
#define LAPIC_SIVR      0x00f0
/* Error Status Register. */
#define LAPIC_ESR       0x0280
/* Interrupt Command Register */
#define LAPIC_ICR_LO    0x0300
#define LAPIC_ICR_HI    0x0310
/* Timer Register */
#define LAPIC_TIMER     0x0320
/* Performance Monitoring Counters Register */
#define LAPIC_PMCR      0x0340
/* Local Vector Table 1 (LINT0) */
#define LAPIC_LINT0     0x0350
/* Local Vector Table 2 (LINT1) */
#define LAPIC_LINT1     0x0360
/* Local Vector Table 3 (ERROR) */
#define LAPIC_ERROR     0x0370
#define LAPIC_TICR      0x0380
#define LAPIC_TCCR      0x0390
#define LAPIC_TDCR      0x03e0

/* Flags for the Spurious Interrupt Vector. */
#define LAPIC_ENABLE    0x00000100

/* Flags for the Interrupt Command Register (low). */
#define LAPIC_INIT      0x00000500
#define LAPIC_STARTUP   0x00000600
#define LAPIC_DELIVERY  0x00001000
#define LAPIC_ASSERT    0x00004000
#define LAPIC_DEASSERT  0x00000000
#define LAPIC_LEVEL     0x00008000
#define LAPIC_BROADCAST 0x00080000
#define LAPIC_OTHERS    0x000c0000
#define LAPIC_BUSY      0x00001000
#define LAPIC_FIXED     0x00000000

/* Flags for the Timer Register. */
#define LAPIC_X1        0x0000000b
#define LAPIC_PERIODIC  0x00020000

/* Flags for Local Vector Table 3. */
#define LAPIC_MASKED    0x00010000

