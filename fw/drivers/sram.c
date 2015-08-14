#include <avr/io.h>
#include <stdio.h>

#include "spi.h"
#include "gpio.h"

#include "sram.h"

void sram_init() {
    printf("sram_init\n");
    
	PORTE.PIN3CTRL |= PORT_OPC_PULLUP_gc; // pull up on CS Hold Pin
	gpio_on(&PORTE, SRAM_CS_PIN_gm);

    // NOTE: this happens two times, once here and once for the POT SPI
    // interface.  We think it's not a big deal but we will see when it's
    // not 10PM on a Thursday.
	PORTC.DIR |= SPI_SS_PIN_bm | SPI_MOSI_PIN_bm | SPI_SCK_PIN_bm;
    PORTE.DIR |= SRAM_CS_PIN_gm;

	SPIC.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_3_gc | SPI_PRESCALER_DIV4_gc; 
}

int sram_self_test() {
    uint8_t command_write[] = { 0x02, 0x00, 0x00, 0xBA };
    uint8_t response_write[4];
    uint8_t command_read[] = { 0x03, 0x00, 0x00, 0xFE };
    uint8_t response_read[4];
    printf("sram_self_test\n");
    gpio_off(&PORTE, SRAM_CS_PIN_gm);
    spi_txrx(&SPIC, command_write, response_write, sizeof(command_write));
    gpio_on(&PORTE, SRAM_CS_PIN_gm);

    gpio_off(&PORTE, SRAM_CS_PIN_gm);
    spi_txrx(&SPIC, command_read, response_read, sizeof(command_read));
    gpio_on(&PORTE, SRAM_CS_PIN_gm);

    printf("sram_response 0x%x\n", response_read[3]);
    return (response_read[3] == 0xBA) ? 0 : 1;
}
