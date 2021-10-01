#include <types.h>
#include <lapic.h>

#include <x86-64/idt.h>

#include <kernel/mem.h>
#include <kernel/sched.h>

physaddr_t lapic_addr = 0;
char *lapic_base = NULL;

uint32_t lapic_read(uint16_t addr)
{
	return *(volatile uint32_t *)(lapic_base + addr);
}

void lapic_write(uint16_t addr, uint32_t value)
{
	*(volatile uint32_t *)(lapic_base + addr) = value;
	lapic_read(LAPIC_ID);
}

void lapic_init(void)
{
	if (!lapic_addr) {
		return;
	}

	/* Map in the 4K MMIO region of the local APIC. */
	if (!lapic_base) {
		lapic_base = mmio_map_region(lapic_addr, PAGE_SIZE);
	}

	/* Enable the local APIC. */
	lapic_write(LAPIC_SIVR, LAPIC_ENABLE | IRQ_SPURIOUS);

	/* Set up a periodic timer using the local APIC. We write the initial
	 * counter to LAPIC_TICR and the local APIC repeatedly counts down at
	 * the bus frequency whereupon it triggers an interrupt.
	 *
	 * Technically, we should callibrate the frequency used according to an
	 * external time source such as the HPET.
	 */
	lapic_write(LAPIC_TDCR, LAPIC_X1);
	lapic_write(LAPIC_TIMER, LAPIC_PERIODIC | IRQ_TIMER);
	lapic_write(LAPIC_TICR, 10000000);

	/* Leave LAPIC_LINT0 of the BSP (bootstrap processor) enabled, such
	 * that it can get interrupts from the 8259A chip.
	 */
	if (this_cpu != boot_cpu) {
		lapic_write(LAPIC_LINT0, LAPIC_MASKED);
	}

	/* Disable NMI (non-maskable interrupts) through LAPIC_LINT1 on all
	 * CPUs.
	 */
	lapic_write(LAPIC_LINT1, LAPIC_MASKED);

	if (((lapic_read(LAPIC_VERSION) >> 16) & 0xff) >= 4) {
		lapic_write(LAPIC_PMCR, LAPIC_MASKED);
	}

	/* Map error interrupts to IRQ_ERROR. */
	lapic_write(LAPIC_ERROR, IRQ_ERROR);

	/* Clear the error status register. This requires back-to-back
	 * writes.
	 */
	lapic_write(LAPIC_ESR, 0);
	lapic_write(LAPIC_ESR, 0);

	/* Acknowledge any outstanding interrupts. */
	lapic_write(LAPIC_EOI, 0);

	/* Broadcast a de-assert to synchronize the arbitration IDs. */
	lapic_write(LAPIC_ICR_HI, 0);
	lapic_write(LAPIC_ICR_LO, LAPIC_BROADCAST | LAPIC_INIT | LAPIC_LEVEL);

	/* Wait for delivery. */
	while (lapic_read(LAPIC_ICR_LO) & LAPIC_DELIVERY)
		;

	/* Enable interrupts on the local APIC by setting the lowest task
	 * priority.
	 */
	lapic_write(LAPIC_TPR, 0);
}

/* Gets the CPU number according to the local APIC. */
int lapic_cpunum(void)
{
	if (!lapic_base) {
		return 0;
	}

	return lapic_read(LAPIC_ID) >> 24;
}

/* Acknowledge the interrupt. */
void lapic_eoi(void)
{
	if (!lapic_base) {
		return;
	}

	lapic_write(LAPIC_EOI, 0);
}

/* Sends an IPI to the other CPU cores. */
void lapic_ipi(int vector)
{
	/* Send the IPI to the other CPU cores. */
	lapic_write(LAPIC_ICR_LO, LAPIC_OTHERS | LAPIC_FIXED | vector);

	/* Wait for the delivery. */
	while (lapic_read(LAPIC_ICR_LO) & LAPIC_DELIVERY)
		;
}

