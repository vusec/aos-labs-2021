#pragma once

void pic_remap(int offset1, int offset2);
uint16_t pic_get_reg(int ocw3);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);
void pic_disable_all_irqs(void);
void pic_enable_irq(int irq);
void pic_disable_irq(int irq);
void pic_eoi(int irq);
void pic_init(void);

