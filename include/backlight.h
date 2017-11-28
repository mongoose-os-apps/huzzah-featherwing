#ifndef __BACKLIGHT_H
#define __BACKLIGHT_H

#include "mgos.h"
#include "mgos_pwm.h"
#include "mgos_config.h"
#include "mgos_timers.h"

#define BACKLIGHT_PWM_HZ 10000
#define BACKLIGHT_STEP_USEC 20000 // 20ms per step.

void backlight_init(void);
void backlight_keepalive();
bool backlight_active();

#endif // __BACKLIGHT_H
