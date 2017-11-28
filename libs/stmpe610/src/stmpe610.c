/*
 *  Author: LoBo (loboris@gmail.com, loboris.github)
 *
 *  Module supporting STMPE touch screen controller.
 * 
 * HIGH SPEED LOW LEVEL DISPLAY FUNCTIONS
 * USING DIRECT or DMA SPI TRANSFER MODEs
 *
*/

#include "mgos.h"
#include "lobo_spi.h"
#include "stmpe610.h"

#define SPI_BUS VSPI_HOST
#define STMPE_SPI_MODE 1


struct mgos_stmpe610_event_data s_last_touch;


static spi_lobo_device_handle_t s_stmpe610_spi = NULL;
static mgos_stmpe610_event_t s_event_handler = NULL;
static enum mgos_stmpe610_rotation_t s_rotation = STMPE610_PORTRAIT;

static void IRAM_ATTR _spi_transfer_start(spi_lobo_device_handle_t spi_dev, int wrbits, int rdbits) {
	// Load send buffer
	spi_dev->host->hw->user.usr_mosi_highpart = 0;
	spi_dev->host->hw->mosi_dlen.usr_mosi_dbitlen = wrbits-1;
	spi_dev->host->hw->user.usr_mosi = 1;
    if (rdbits) {
        spi_dev->host->hw->miso_dlen.usr_miso_dbitlen = rdbits;
        spi_dev->host->hw->user.usr_miso = 1;
    }
    else {
        spi_dev->host->hw->miso_dlen.usr_miso_dbitlen = 0;
        spi_dev->host->hw->user.usr_miso = 0;
    }
	// Start transfer
	spi_dev->host->hw->cmd.usr = 1;
    // Wait for SPI bus ready
	while (spi_dev->host->hw->cmd.usr);
}

static void IRAM_ATTR stmpe610_write_reg(uint8_t reg, uint8_t val) {

    spi_lobo_device_select(s_stmpe610_spi, 0);

    s_stmpe610_spi->host->hw->data_buf[0] = (val << 8) | reg;
    _spi_transfer_start(s_stmpe610_spi, 16, 0);

    spi_lobo_device_deselect(s_stmpe610_spi);
}

static uint8_t IRAM_ATTR stmpe610_read_byte(uint8_t reg) {
    spi_lobo_device_select(s_stmpe610_spi, 0);

    s_stmpe610_spi->host->hw->data_buf[0] = (reg << 8) | (reg | 0x80);
    _spi_transfer_start(s_stmpe610_spi, 16, 16);
    uint8_t res = s_stmpe610_spi->host->hw->data_buf[0] >> 8;

    spi_lobo_device_deselect(s_stmpe610_spi);
    return res;
}

static uint16_t IRAM_ATTR stmpe610_read_word(uint8_t reg) {
    spi_lobo_device_select(s_stmpe610_spi, 0);

    s_stmpe610_spi->host->hw->data_buf[0] = ((((reg+1) << 8) | ((reg+1) | 0x80)) << 16) | (reg << 8) | (reg | 0x80);
    _spi_transfer_start(s_stmpe610_spi, 32, 32);
    uint16_t res = (uint16_t)(s_stmpe610_spi->host->hw->data_buf[0] & 0xFF00);
    res |= (uint16_t)(s_stmpe610_spi->host->hw->data_buf[0] >> 24);

    spi_lobo_device_deselect(s_stmpe610_spi);
    return res;
}

static uint32_t stmpe610_getID()
{
    uint16_t tid = stmpe610_read_word(0);
    uint8_t tver = stmpe610_read_byte(2);
    return (tid << 8) | tver;
}


static uint8_t stmpe610_bufferLength(void) {
  return stmpe610_read_byte(STMPE_FIFO_SIZE);
}


static uint8_t stmpe610_readData(uint16_t *x, uint16_t *y, uint8_t *z) {
  uint8_t samples, cnt;
  uint32_t sum_sample_x = 0, sum_sample_y = 0;
  uint16_t sum_sample_z = 0;

  samples = cnt = stmpe610_bufferLength();
  LOG(LL_DEBUG, ("Touch sensed with %d samples", samples));
  if (samples == 0)
    return 0;
  
  while (cnt>0) {
    uint16_t sample_x, sample_y;
    uint8_t sample_z;

    sample_x = stmpe610_read_word(0x4D);
    sample_y = stmpe610_read_word(0x4F);
    sample_z = stmpe610_read_byte(0x51);

    sum_sample_x += sample_x;
    sum_sample_y += sample_y;
    sum_sample_z += sample_z;
    LOG(LL_DEBUG, ("Sample at (%d,%d) pressure=%d, bufferLength=%d", sample_x, sample_y, sample_z, stmpe610_bufferLength()));
    cnt--;
  }
  *x = sum_sample_x / samples;
  *y = sum_sample_y / samples;
  *z = sum_sample_z / samples;

  stmpe610_write_reg(STMPE_FIFO_STA, STMPE_FIFO_STA_RESET); // clear FIFO
  stmpe610_write_reg(STMPE_FIFO_STA, 0);    // unreset

  return samples;
}

static long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  if (x<in_min) x=in_min;
  if (x>in_max) x=in_max;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void stmpe610_map_rotation(uint16_t x, uint16_t y, uint16_t *x_out, uint16_t *y_out) {
  switch(s_rotation) {
    case STMPE610_LANDSCAPE:
      *x_out = map(y, 150, 3800, 0, 4095);
      *y_out = map(x, 250, 3700, 0, 4095);
      break;
    case STMPE610_PORTRAIT_FLIP:
      *x_out = map(x, 250, 3800, 0, 4095);
      *y_out = 4095-map(y, 150, 3700, 0, 4095);
      break;
    case STMPE610_LANDSCAPE_FLIP:
      *x_out = 4095-map(y, 150, 3800, 0, 4095);
      *y_out = 4095-map(x, 250, 3700, 0, 4095);
      break;
    default: // STMPE610_PORTRAIT
      *x_out = 4095-map(x, 250, 3800, 0, 4095);
      *y_out = map(y, 150, 3700, 0, 4095);
  }
}

/* Each time a TOUCH_DOWN event occurs, s_last_touch is populated
   and a timer is started. When the timer fires, we checke to see
   if we've sent a TOUCH_UP event already. If not, we may still be
   pressing the screen. IF we're not pressing the screen STMP610
   bufferLength() will be 0. We've now detected a DOWN without a
   corresponding UP, so we send it ourselves.
*/
static void stmpe610_down_cb(void *arg) {
  if (s_last_touch.direction == TOUCH_UP)
    return;
  if (stmpe610_bufferLength() > 0)
    return;

  s_last_touch.direction=TOUCH_UP;
  if (s_event_handler) {
    LOG(LL_INFO, ("Touch DOWN not followed by UP -- sending phantom UP"));
    s_event_handler(&s_last_touch);
  }
}

static void stmpe610_irq(int pin, void *arg) {
  struct mgos_stmpe610_event_data ed;
  uint16_t x, y; 
  uint8_t z; 

  if (stmpe610_bufferLength()==0) {
    uint8_t i;
    LOG(LL_DEBUG, ("Touch DOWN"));
    for (i=0; i<10; i++) {
      mgos_msleep(5);
      if (stmpe610_bufferLength()>0) {

        stmpe610_readData(&x, &y, &z);
        LOG(LL_DEBUG, ("Touch DOWN at (%d,%d) pressure=%d, length=%d, iteration=%d", x, y, z, ed.length, i));

        ed.length=1;
        ed.direction = TOUCH_DOWN;
        stmpe610_map_rotation(x, y, &ed.x, &ed.y);
        ed.z = z;
        // To avoid DOWN events without an UP event, set a timer (see stmpe610_down_cb for details)
        memcpy((void *)&s_last_touch, (void *)&ed, sizeof(s_last_touch));
        mgos_set_timer(100, 0, stmpe610_down_cb, NULL);
        if (s_event_handler)
          s_event_handler(&ed);
        break;
      }
    }
    stmpe610_write_reg(STMPE_INT_STA, 0xFF); // reset all ints
    return;
  }

  ed.length = stmpe610_readData(&x, &y, &z);
  LOG(LL_DEBUG, ("Touch UP at (%d,%d) pressure=%d, length=%d", x, y, z, ed.length));
  ed.direction = TOUCH_UP;
  stmpe610_map_rotation(x, y, &ed.x, &ed.y);
  ed.z = z;
  memcpy((void *)&s_last_touch, (void *)&ed, sizeof(s_last_touch));
  if (s_event_handler)
    s_event_handler(&ed);

  stmpe610_write_reg(STMPE_INT_STA, 0xFF); // reset all ints
  (void) pin;
  (void) arg;
}

void mgos_stmpe610_set_handler(mgos_stmpe610_event_t handler) {
  s_event_handler = handler;
}

void mgos_stmpe610_set_rotation(enum mgos_stmpe610_rotation_t rotation) {
  s_rotation = rotation;
}

bool mgos_stmpe610_is_touching() {
  return s_last_touch.direction == TOUCH_DOWN;
}

bool mgos_stmpe610_init(void) {
  esp_err_t ret;

  gpio_pad_select_gpio(mgos_sys_config_get_stmpe610_cs_pin());
  gpio_set_direction(mgos_sys_config_get_stmpe610_cs_pin(), GPIO_MODE_OUTPUT);

  spi_lobo_bus_config_t buscfg={
    .miso_io_num=mgos_sys_config_get_spi_miso(),
    .mosi_io_num=mgos_sys_config_get_spi_mosi(),
    .sclk_io_num=mgos_sys_config_get_spi_sck(),
    .quadwp_io_num=-1,
    .quadhd_io_num=-1,
    .max_transfer_sz = 6*1024,
  };

  spi_lobo_device_interface_config_t tsdevcfg={
    .clock_speed_hz=1000000,
    .mode=STMPE_SPI_MODE,
    .spics_io_num=mgos_sys_config_get_stmpe610_cs_pin(),              //Touch CS pin
    .spics_ext_io_num=-1,                   //Not using the external CS
    .flags = 0,
  };

  ret=spi_lobo_bus_add_device(SPI_BUS, &buscfg, &tsdevcfg, &s_stmpe610_spi);
  assert(ret==ESP_OK);

  ret = spi_lobo_device_select(s_stmpe610_spi, 1);
  assert(ret==ESP_OK);
  ret = spi_lobo_device_deselect(s_stmpe610_spi);
  assert(ret==ESP_OK);

  uint32_t tver = stmpe610_getID();
  if (tver >> 8 != 0x0811) {
    LOG(LL_ERROR, ("STMPE SPI init failed, disabling"));
    return true;
  }
  LOG(LL_INFO, ("SPI init ok (cs=%d, speed=%u, ver=%04x, rev=%02x); irq=%d", mgos_sys_config_get_stmpe610_cs_pin(), spi_lobo_get_speed(s_stmpe610_spi),
     tver>>8, tver&0xFF, mgos_sys_config_get_stmpe610_irq_pin()));

  stmpe610_write_reg(STMPE_SYS_CTRL1, STMPE_SYS_CTRL1_RESET);
  mgos_msleep(10);
  
  for (uint8_t i=0; i<65; i++)
    stmpe610_read_byte(i);

  stmpe610_write_reg(STMPE_SYS_CTRL2, 0x0); // turn on clocks!
  stmpe610_write_reg(STMPE_INT_EN, STMPE_INT_EN_TOUCHDET);
  stmpe610_write_reg(STMPE_ADC_CTRL1, STMPE_ADC_CTRL1_10BIT | (0x6 << 4)); // 96 clocks per conversion
  stmpe610_write_reg(STMPE_ADC_CTRL2, STMPE_ADC_CTRL2_6_5MHZ);
  stmpe610_write_reg(STMPE_TSC_CFG, STMPE_TSC_CFG_4SAMPLE | STMPE_TSC_CFG_DELAY_1MS | STMPE_TSC_CFG_SETTLE_5MS);
  stmpe610_write_reg(STMPE_TSC_FRACTION_Z, 0x6);
  stmpe610_write_reg(STMPE_FIFO_TH, 1);
  stmpe610_write_reg(STMPE_FIFO_STA, STMPE_FIFO_STA_RESET);
  stmpe610_write_reg(STMPE_FIFO_STA, 0);    // unreset
  stmpe610_write_reg(STMPE_TSC_I_DRIVE, STMPE_TSC_I_DRIVE_50MA);
  stmpe610_write_reg(STMPE_TSC_CTRL, 0x30);         // X&Y&Z, 16 reading window
  stmpe610_write_reg(STMPE_TSC_CTRL, 0x31);         // X&Y&Z, 16 reading window, TSC enable
  stmpe610_write_reg(STMPE_INT_STA, 0xFF); // reset all ints
  stmpe610_write_reg(STMPE_INT_CTRL, STMPE_INT_CTRL_POL_LOW | STMPE_INT_CTRL_EDGE | STMPE_INT_CTRL_ENABLE);

  mgos_gpio_set_mode(mgos_sys_config_get_stmpe610_irq_pin(), MGOS_GPIO_MODE_INPUT);
  mgos_gpio_set_pull(mgos_sys_config_get_stmpe610_irq_pin(), MGOS_GPIO_PULL_UP);
  mgos_gpio_set_int_handler(mgos_sys_config_get_stmpe610_irq_pin(), MGOS_GPIO_INT_EDGE_NEG, stmpe610_irq, NULL);
  mgos_gpio_enable_int(mgos_sys_config_get_stmpe610_irq_pin());

  // Initialize the last touch to TOUCH_UP
  s_last_touch.direction=TOUCH_UP;
  s_last_touch.x=0;
  s_last_touch.y=0;
  s_last_touch.z=0;
  s_last_touch.length=0;
  return true;
}
