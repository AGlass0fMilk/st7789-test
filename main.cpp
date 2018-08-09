/*
 * main.cpp
 *
 *  Created on: Aug 8, 2018
 *      Author: gdbeckstein
 */

#include "drivers/SPI.h"
#include "drivers/DigitalOut.h"

#include "platform/mbed_debug.h"
#include "platform/mbed_wait_api.h"

#include <stdint.h>

#define ST77XX_NOP     0x00
#define ST77XX_SWRESET 0x01
#define ST77XX_RDDID   0x04
#define ST77XX_RDDST   0x09

#define ST77XX_SLPIN   0x10
#define ST77XX_SLPOUT  0x11
#define ST77XX_PTLON   0x12
#define ST77XX_NORON   0x13

#define ST77XX_INVOFF  0x20
#define ST77XX_INVON   0x21
#define ST77XX_DISPOFF 0x28
#define ST77XX_DISPON  0x29
#define ST77XX_CASET   0x2A
#define ST77XX_RASET   0x2B
#define ST77XX_RAMWR   0x2C
#define ST77XX_RAMRD   0x2E

#define ST77XX_PTLAR   0x30
#define ST77XX_COLMOD  0x3A
#define ST77XX_MADCTL  0x36

#define ST77XX_MADCTL_MY  0x80
#define ST77XX_MADCTL_MX  0x40
#define ST77XX_MADCTL_MV  0x20
#define ST77XX_MADCTL_ML  0x10
#define ST77XX_MADCTL_RGB 0x00

#define ST77XX_RDID1   0xDA
#define ST77XX_RDID2   0xDB
#define ST77XX_RDID3   0xDC
#define ST77XX_RDID4   0xDD

#define DISPLAY_SCLK 	p27
#define DISPLAY_MOSI 	p26
#define DISPLAY_CS	 	p2
#define DISPLAY_RESET	p25
#define DISPLAY_DC		p24

mbed::SPI spi(DISPLAY_MOSI, NC, DISPLAY_SCLK);
mbed::DigitalOut display_reset(DISPLAY_RESET, 1);
mbed::DigitalOut display_dc(DISPLAY_DC, 0);
mbed::DigitalOut display_cs(DISPLAY_CS, 1);

void write_command(int cmd)
{
	display_cs = 0;
	display_dc = 0;
	spi.write(cmd);
	display_cs = 1;
}

void write_command_with_params(int cmd, const uint8_t* params, uint8_t len)
{
	display_cs = 0;
	display_dc = 0;
	spi.write(cmd);
	display_dc = 1;
	spi.write((const char*) params, len, NULL, 0);
	display_cs = 1;
}

void write_data(const uint8_t* data, uint8_t len)
{
	display_cs = 0;
	display_dc = 1;
	spi.write((const char*) data, len, NULL, 0);
	display_cs = 1;
}

int main(void)
{
	spi.frequency(1000000);
	display_reset = 0;
	wait_ms(10);
	display_reset = 1;
	wait_ms(100);
	display_dc = 0;

	// Software reset
	write_command(ST77XX_SWRESET);
	wait_ms(200);
	// Out of sleep mode
	write_command(ST77XX_SLPOUT);
	wait_ms(500);

	uint8_t buf[8];

	// Set color mode
	buf[0] = 0x55;
	write_command_with_params(ST77XX_COLMOD, buf, 1);
	wait_ms(10);

	// Set memory access control
	buf[0] = 0x08;
	write_command_with_params(ST77XX_MADCTL, buf, 1);

	// Set column address
	buf[0] = 0x00; // Start address high byte
	buf[1] = 0x00; // Start address low byte
	buf[2] = 0x00; // End address high byte
	buf[3] = 0xF0; // End address low byte
	write_command_with_params(ST77XX_CASET, buf, 4);

	// Set row address
	write_command_with_params(ST77XX_RASET, buf, 4);

	// Normal display on
	write_command(ST77XX_NORON);
	wait_ms(10);

	// Display on
	write_command(ST77XX_DISPON);
	wait_ms(500);

	while(true)
	{
		//
	}
}
