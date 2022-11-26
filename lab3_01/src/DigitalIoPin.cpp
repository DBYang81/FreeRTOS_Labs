/*
 * DigitalIoPin.cpp
 *
 *  Created on: 14 Nov 2022
 *      Author: krl
 */

#include "DigitalIoPin.h"
#include "chip.h"

DigitalIoPin::DigitalIoPin(int port_, int pin_, pinMode mode, bool invert) : port(port_), pin(pin_), inv(invert){
	// TODO Auto-generated constructor stub
	if(mode == output){
		Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, IOCON_MODE_INACT | IOCON_DIGMODE_EN);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
	}else{
		uint32_t pm = IOCON_DIGMODE_EN;
		if(invert) pm |= IOCON_INV_EN; // pm = IOCON_DIGMODE_EN | IOCON_INV_EN;
		if(mode == pullup){
			pm |= IOCON_MODE_PULLUP; // pm = IOCON_DIGMODE_EN | (IOCON_INV_EN) | IOCON_MODE_PULLUP;
		}else if(mode == pulldown){
			pm |= IOCON_MODE_PULLDOWN; //pm = IOCON_DIGMODE_EN | (IOCON_INV_EN) | IOCON_MODE_PULLDOWN;
		}
		Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, pm);
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
	}
}

DigitalIoPin::~DigitalIoPin() {
	// TODO Auto-generated destructor stub
}

bool DigitalIoPin::read(){
	return Chip_GPIO_GetPinState(LPC_GPIO, port, pin);
}

void DigitalIoPin::write(bool value){
	return Chip_GPIO_SetPinState(LPC_GPIO, port, pin, inv ^ value);
}
