#pragma once

#include <lib.h>

int rtc_busy(void);
void rtc_get_time(struct tm *tm);

