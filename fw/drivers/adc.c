#include <avr/io.h>
#include <stddef.h>
#include <stdio.h>

#include "adc.h"
#include "nvm.h"
#include "timer.h"

#define ADC_CAL_NUM 100

void adc_init(ADC_t* adc)
{
	adc->CTRLB = ADC_CONMODE_bm | ADC_RESOLUTION_12BIT_gc; // 12bit, signed (sadly, have to do signed and lose a bit since differential has much less noise and it's only available in signed)
	adc->REFCTRL = ADC_REFSEL_AREFA_gc; // external reference (2.048 V) on PORTA
	adc->PRESCALER = ADC_PRESCALER_DIV16_gc; // set ADC clock to 2MHz for some reason it works best here

	// get the factory written ADC calibration value
	uint16_t adc_cal = 0;
	if (adc == &ADCA)
	{
		adc_cal =  nvm_read_calibration_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0));
		adc_cal |= nvm_read_calibration_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1)) << 8;
	}
	if (adc == &ADCB)
	{
		adc_cal =  nvm_read_calibration_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL0));
		adc_cal |= nvm_read_calibration_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL1)) << 8; 
	}
	adc->CAL = adc_cal; // load the calibration value into the ADC adc_cal

	adc->CH0.CTRL = ADC_CH_INPUTMODE_DIFF_gc; // no gain needed due to low impedience
	adc->CH0.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc | ADC_CH_MUXNEG_GND_MODE3_gc;

	adc->EVCTRL = ADC_EVSEL_0123_gc | ADC_SWEEP_0_gc | ADC_EVACT_SYNCSWEEP_gc; // read channel 0 for event 0 

	adc->CTRLA = ADC_ENABLE_bm; // turn on the ADC

	// wait for the adc to settle
	timer_sleep_ms(10);
}

int16_t adc_sample(ADC_t* adc, uint8_t channel)
{
	adc->CTRLA |= (ADC_CH0START_bm << channel);
	loop_until_bit_is_set(adc->INTFLAGS, channel);
	return *(&adc->CH0RES + channel);
}
