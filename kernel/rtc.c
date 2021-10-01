#include <x86-64/asm.h>

#include <lib.h>

#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

int rtc_busy(void)
{
	outb(CMOS_ADDR, 0x0a);

	return inb(CMOS_DATA) & 0x80;
}

uint8_t rtc_read_reg(int reg)
{
	outb(CMOS_ADDR, reg);

	return inb(CMOS_DATA);
}

uint8_t bcd_to_int(uint8_t val)
{
	return (val & 0x0f) + (val / 16) * 10;
}

void rtc_get_time(struct tm *tm)
{
	uint8_t sec, min, hour, day, mon, year, reg_b;
	
	while (rtc_busy());

	sec = rtc_read_reg(0x00);
	min = rtc_read_reg(0x02);
	hour = rtc_read_reg(0x04);
	day = rtc_read_reg(0x07);
	mon = rtc_read_reg(0x08);
	year = rtc_read_reg(0x09);

	reg_b = rtc_read_reg(0x0b);

	if (!(reg_b & 0x04)) {
		sec = bcd_to_int(sec);
		min = bcd_to_int(min);
		hour = bcd_to_int(hour);
		day = bcd_to_int(day);
		mon = bcd_to_int(mon);
		year = bcd_to_int(year);
	}

	if (!(reg_b & 0x02) && (hour & 0x80)) {
		hour = ((hour & 0x7f) + 12) % 24;
	}

	tm->tm_sec = sec;
	tm->tm_min = min;
	tm->tm_hour = hour;
	tm->tm_mday = day;
	tm->tm_mon = mon - 1;
	tm->tm_year = 100 + year;
}

