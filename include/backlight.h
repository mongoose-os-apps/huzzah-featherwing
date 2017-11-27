#ifndef __BACKLIGHT_H
#define __BACKLIGHT_H

#include "mgos.h"
#include "mgos_pwm.h"
#include "mgos_config.h"
#include "mgos_timers.h"

#define BACKLIGHT_PWM_HZ 10000
#define BACKLIGHT_STEP_USEC 10000 // 10ms per step.

void backlight_init(void);
void backlight_set(float new_duty, int fader_msec);
void backlight_keepalive();
bool backlight_active();

#endif // __BACKLIGHT_H
