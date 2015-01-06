#include <avr/io.h>

#include "clock.h"

void clock_set_crystal()
{
	OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc; // set parameters for the crystal 
	OSC.CTRL |= OSC_XOSCEN_bm; // enable the crystal 

	OSC.PLLCTRL = OSC_PLLSRC_XOSC_gc | (2<<OSC_PLLFAC_gp); // setup 2x PLL

	loop_until_bit_is_set(OSC.STATUS, OSC_XOSCRDY_bp); // wait for the crystal to stabalize

	OSC.CTRL |= OSC_PLLEN_bm; // enable the PLL

	loop_until_bit_is_set(OSC.STATUS, OSC_PLLRDY_bp); // wait for the PLL to start

	CPU_CCP = CCP_IOREG_gc; // let me change this protected register
	CLK.CTRL = CLK_SCLKSEL_PLL_gc; // set the system clock to the PLL

	CPU_CCP = CCP_IOREG_gc; // let me change this protected register
	CLK.PSCTRL = CLK_PSADIV_1_gc | CLK_PSBCDIV_1_1_gc; // set all prescalers to 1, perpherials get the highest rate

	OSC.CTRL = OSC_XOSCEN_bm | OSC_PLLEN_bm; // only enable the crystal and the PLL (disable all other oscillators)
}
