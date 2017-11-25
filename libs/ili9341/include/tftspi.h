/*
 * 
 * HIGH SPEED LOW LEVEL DISPLAY FUNCTIONS USING DIRECT TRANSFER MODE
 *
*/

#ifndef _TFTSPI_H_
#define _TFTSPI_H_

#include "tftspi.h"
#include "lobo_spi.h"
#include "sdkconfig.h"

// === Screen orientation constants ===
#define PORTRAIT	0
#define LANDSCAPE	1
#define PORTRAIT_FLIP	2
#define LANDSCAPE_FLIP	3

// ** Set the correct configuration for Adafruit TFT Feather
// ---------------------------------------------------------
#define DEFAULT_TFT_DISPLAY_WIDTH   240
#define DEFAULT_TFT_DISPLAY_HEIGHT  320
#define DISP_COLOR_BITS_24          0x66
#define DEFAULT_GAMMA_CURVE         0
#define DEFAULT_SPI_CLOCK           26000000
#define TFT_RGB_BGR                 0x08

// ---------------------------------------------------------

// ##############################################################
// #### Global variables                                     ####
// ##############################################################

// ==== Spi clock for reading data from display memory in Hz ====
extern uint32_t max_rdclock;

// ==== Display dimensions in pixels ============================
extern int _width;
extern int _height;

// ==== Spi device handles for display and touch screen =========
extern spi_lobo_device_handle_t disp_spi;

// ##############################################################

// 24-bit color type structure
typedef struct __attribute__((__packed__)) {
//typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color_t ;

// ==== Display commands constants ====
#define TFT_INVOFF     0x20
#define TFT_INVONN     0x21
#define TFT_DISPOFF    0x28
#define TFT_DISPON     0x29
#define TFT_MADCTL	   0x36
#define TFT_PTLAR 	   0x30
#define TFT_ENTRYM 	   0xB7

#define TFT_CMD_NOP			0x00
#define TFT_CMD_SWRESET		0x01
#define TFT_CMD_RDDID		0x04
#define TFT_CMD_RDDST		0x09

#define TFT_CMD_SLPIN		0x10
#define TFT_CMD_SLPOUT		0x11
#define TFT_CMD_PTLON		0x12
#define TFT_CMD_NORON		0x13

#define TFT_CMD_RDMODE		0x0A
#define TFT_CMD_RDMADCTL	0x0B
#define TFT_CMD_RDPIXFMT	0x0C
#define TFT_CMD_RDIMGFMT	0x0D
#define TFT_CMD_RDSELFDIAG  0x0F

#define TFT_CMD_GAMMASET	0x26

#define TFT_CMD_FRMCTR1		0xB1
#define TFT_CMD_FRMCTR2		0xB2
#define TFT_CMD_FRMCTR3		0xB3
#define TFT_CMD_INVCTR		0xB4
#define TFT_CMD_DFUNCTR		0xB6

#define TFT_CMD_PWCTR1		0xC0
#define TFT_CMD_PWCTR2		0xC1
#define TFT_CMD_PWCTR3		0xC2
#define TFT_CMD_PWCTR4		0xC3
#define TFT_CMD_PWCTR5		0xC4
#define TFT_CMD_VMCTR1		0xC5
#define TFT_CMD_VMCTR2		0xC7

#define TFT_CMD_RDID1		0xDA
#define TFT_CMD_RDID2		0xDB
#define TFT_CMD_RDID3		0xDC
#define TFT_CMD_RDID4		0xDD

#define TFT_CMD_GMCTRP1		0xE0
#define TFT_CMD_GMCTRN1		0xE1

#define TFT_CMD_POWERA		0xCB
#define TFT_CMD_POWERB		0xCF
#define TFT_CMD_POWER_SEQ	0xED
#define TFT_CMD_DTCA		0xE8
#define TFT_CMD_DTCB		0xEA
#define TFT_CMD_PRC			0xF7
#define TFT_CMD_3GAMMA_EN	0xF2

#define ST_CMD_VCOMS       0xBB
#define ST_CMD_FRCTRL2      0xC6
#define ST_CMD_PWCTR1		0xD0

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_MH  0x04

#define TFT_CASET		0x2A
#define TFT_PASET		0x2B
#define TFT_RAMWR		0x2C
#define TFT_RAMRD		0x2E
#define TFT_CMD_PIXFMT	0x3A

#define TFT_CMD_DELAY	0x80


// Initialization sequence for ILI7341
// ====================================
static const uint8_t ILI9341_init[] = {
  24,                   					        // 24 commands in list
  TFT_CMD_SWRESET, TFT_CMD_DELAY,					//  1: Software reset, no args, w/delay
  250,												//     200 ms delay
  TFT_CMD_POWERA, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  TFT_CMD_POWERB, 3, 0x00, 0XC1, 0X30,
  0xEF, 3, 0x03, 0x80, 0x02,
  TFT_CMD_DTCA, 3, 0x85, 0x00, 0x78,
  TFT_CMD_DTCB, 2, 0x00, 0x00,
  TFT_CMD_POWER_SEQ, 4, 0x64, 0x03, 0X12, 0X81,
  TFT_CMD_PRC, 1, 0x20,
  TFT_CMD_PWCTR1, 1, 0x23,							//Power control VRH[5:0]
  TFT_CMD_PWCTR2, 1, 0x10,							//Power control SAP[2:0];BT[3:0]
  TFT_CMD_VMCTR1, 2, 0x3e, 0x28,					//VCM control
  TFT_CMD_VMCTR2, 1, 0x86,							//VCM control2
  TFT_MADCTL, 1,									// Memory Access Control (orientation)
  (MADCTL_MX | TFT_RGB_BGR),
  // *** INTERFACE PIXEL FORMAT: 0x66 -> 18 bit; 0x55 -> 16 bit
  TFT_CMD_PIXFMT, 1, DISP_COLOR_BITS_24,
  TFT_INVOFF, 0,
  TFT_CMD_FRMCTR1, 2, 0x00, 0x18,
  TFT_CMD_DFUNCTR, 4, 0x08, 0x82, 0x27, 0x00,		// Display Function Control
  TFT_PTLAR, 4, 0x00, 0x00, 0x01, 0x3F,
  TFT_CMD_3GAMMA_EN, 1, 0x00,						// 3Gamma Function: Disable (0x02), Enable (0x03)
  TFT_CMD_GAMMASET, 1, 0x01,						//Gamma curve selected (0x01, 0x02, 0x04, 0x08)
  TFT_CMD_GMCTRP1, 15,   							//Positive Gamma Correction
  0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  TFT_CMD_GMCTRN1, 15,   							//Negative Gamma Correction
  0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  TFT_CMD_SLPOUT, TFT_CMD_DELAY,					//  Sleep out
  200,			 									//  120 ms delay
  TFT_DISPON, TFT_CMD_DELAY, 200,
};

// ==== Public functions =========================================================

// == Low level functions; usually not used directly ==
esp_err_t wait_trans_finish(uint8_t free_line);
void disp_spi_transfer_cmd(int8_t cmd);
void disp_spi_transfer_cmd_data(int8_t cmd, uint8_t *data, uint32_t len);
void drawPixel(int16_t x, int16_t y, color_t color, uint8_t sel);
void send_data(int x1, int y1, int x2, int y2, uint32_t len, color_t *buf);
void mgos_ili9341_pushColorRep(int x1, int y1, int x2, int y2, color_t data, uint32_t len);
int read_data(int x1, int y1, int x2, int y2, int len, uint8_t *buf, uint8_t set_sp);
color_t readPixel(int16_t x, int16_t y);
int touch_get_data(uint8_t type);


// Deactivate display's CS line
//========================
esp_err_t disp_deselect();

// Activate display's CS line and configure SPI interface if necessary
//======================
esp_err_t disp_select();


// Find maximum spi clock for successful read from display RAM
// ** Must be used AFTER the display is initialized **
//======================
uint32_t find_rd_speed();


// Change the screen rotation.
// Input: m new rotation value (0 to 3)
//=================================
void _tft_setRotation(uint8_t rot);


// Perform display initialization sequence
// Sets orientation to landscape; clears the screen
// * All pins must be configured
// * SPI interface must already be setup
//======================
void mgos_ili9341_display_init();

// ===============================================================================

#endif
