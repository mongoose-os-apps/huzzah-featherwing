#include "backlight.h"

static float backlight_duty;
static float backlight_target_duty;
static float backlight_step;

static mgos_timer_id backlight_fader_timer_id = 0;
static mgos_timer_id backlight_keepalive_timer_id = 0;

static void backlight_fader_cb(void *arg) {
  float new_duty = backlight_duty+backlight_step;

  LOG(LL_DEBUG,("Set backlight from %.2f to %.2f, step %.4f, target %.2f", backlight_duty, new_duty, backlight_step, backlight_target_duty));
  backlight_duty = new_duty;
  if (backlight_duty>1.0) backlight_duty=1.0;
  if (backlight_duty<0.0) backlight_duty=0.0;
  mgos_pwm_set(mgos_sys_config_get_app_backlight_pin(), BACKLIGHT_PWM_HZ, backlight_duty);
  if (backlight_step<0 && backlight_duty>backlight_target_duty)
    return;
  if (backlight_step>0 && backlight_duty<backlight_target_duty) 
    return;

  mgos_clear_timer(backlight_fader_timer_id);
  backlight_fader_timer_id=0;
  backlight_step=0.0;
  backlight_target_duty=backlight_duty;
  LOG(LL_INFO,("Backlight target reached, set at %.2f", backlight_duty));
  (void) arg;
}


void backlight_set(float new_duty, int fader_msec) {
  if (new_duty<0.0) backlight_target_duty=0.0;
  else if (new_duty>1.0) backlight_target_duty=1.0;
  else backlight_target_duty=new_duty;

  if (backlight_target_duty == backlight_duty)
    return;

  if (fader_msec == 0) {
    backlight_duty=backlight_target_duty;
    backlight_step=0.0;
    LOG(LL_INFO, ("Setting backlight to %.2f", backlight_duty));
    mgos_pwm_set(mgos_sys_config_get_app_backlight_pin(), BACKLIGHT_PWM_HZ, backlight_duty);
    return;
  }

  backlight_step = (backlight_target_duty - backlight_duty) / ((1000.0*fader_msec)/BACKLIGHT_STEP_USEC);
  LOG(LL_INFO, ("Changing backlight from %.2f to %.2f in %d msecs, steps of %.4f", backlight_duty, backlight_target_duty, fader_msec, backlight_step));
  if (backlight_fader_timer_id)
    mgos_clear_timer(backlight_fader_timer_id);
  backlight_fader_timer_id = mgos_set_timer(BACKLIGHT_STEP_USEC/1000, true, backlight_fader_cb, NULL);
}

static void backlight_keepalive_cb(void *arg) {
  LOG(LL_INFO, ("Inactivity timeout of %d seconds reached, setting backlight off", mgos_sys_config_get_app_inactivity_timeout()));
  backlight_set(0.0, 1000);
  (void) arg;
}

void backlight_keepalive() {
  if (backlight_keepalive_timer_id)
    mgos_clear_timer(backlight_keepalive_timer_id);
  backlight_keepalive_timer_id = mgos_set_timer(mgos_sys_config_get_app_inactivity_timeout()*1000, false, backlight_keepalive_cb, NULL);
  backlight_set(1.0, 0);
}

bool backlight_active() {
  return (backlight_duty>0.0);
}


void backlight_init(void) {
  backlight_keepalive();
  return;
}
