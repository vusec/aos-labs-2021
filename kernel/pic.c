#include <types.h>
#include <pic.h>

#include <x86-64/asm.h>
#include <x86-64/idt.h>

/* Initialization code for the PIC. */
void pic_remap(int offset1, int offset2)
{
	unsigned char masks[2];

	/* Save the masks. */
	masks[0] = inb(PIC1_DATA);
	masks[1] = inb(PIC2_DATA);

	/* Start the initialization sequence. */
	outb(PIC1_CMD, PIC_ICW1_INIT | PIC_ICW1_ICW4);
	outb(PIC2_CMD, PIC_ICW1_INIT | PIC_ICW1_ICW4);

	/* ICW2: PIC vector offsets */
	outb(PIC1_DATA, offset1);
	outb(PIC2_DATA, offset2);

	/* ICW3: the slave PIC is at IRQ 2 */
	outb(PIC1_DATA, 1 << IRQ_SLAVE);
	outb(PIC2_DATA, IRQ_SLAVE);

	/* ICW4: configure 8086 mode and enable automatic EOIs on the master */
	outb(PIC1_DATA, PIC_ICW4_8086 | PIC_ICW4_AUTO);
	outb(PIC2_DATA, PIC_ICW4_8086);

	/* Restore the masks. */
	outb(PIC1_DATA, masks[0]);
	outb(PIC2_DATA, masks[1]);
}

/* Helper function to read the OCW3 register from both PICs. */
uint16_t pic_get_reg(int ocw3)
{
	outb(PIC1_CMD, ocw3);
	outb(PIC2_CMD, ocw3);

	return (inb(PIC2_CMD) << 8) | inb(PIC1_CMD);
}

/* Read the IRQ Request Register. */
uint16_t pic_get_irr(void)
{
	return pic_get_reg(PIC_READ_IRR);
}

/* Read the In-Service Register. */
uint16_t pic_get_isr(void)
{
	return pic_get_reg(PIC_READ_ISR);
}

/* Disables all IRQ lines. */
void pic_disable_all_irqs(void)
{
	outb(PIC1_DATA, 0xff);
	outb(PIC2_DATA, 0xff);
}

/* Enables an IRQ line. */
void pic_enable_irq(int irq)
{
	uint16_t port;

	if (irq < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		irq -= 8;
	}

	outb(port, inb(port) & ~(1 << irq));
}

/* Disables an IRQ line. */
void pic_disable_irq(int irq)
{
	uint16_t port;

	if (irq < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		irq -= 8;
	}

	outb(port, inb(port) | (1 << irq));
}

/* Acknowledges the IRQ to the PIC. */
void pic_eoi(int irq)
{
	if (irq >= 8) {
		outb(PIC2_CMD, PIC_EOI);
	}

	outb(PIC1_CMD, PIC_EOI);
}

void pic_init(void)
{
	/* Disable all IRQs to use the local APIC. */
	pic_disable_all_irqs();

	/* Initialize the PIC. */
	pic_remap(IRQ_OFFSET, IRQ_OFFSET + 8);

	/* Read the IRQ Request Register. */
	pic_get_irr();

	/* Enable the slave IRQ. */
	pic_enable_irq(IRQ_SLAVE);
}

