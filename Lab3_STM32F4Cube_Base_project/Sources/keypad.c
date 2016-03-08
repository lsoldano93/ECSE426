#include "keypad.h"

uint8_t get_column(void) {
	// if column activated its set to active low
}

uint8_t get_row(void) {
	// if row activated its set to active low
	if(!HAL_GPIO__ReadPin(GPIOD, GPIO_PIN_2)) {
		return 1;
	}
	else {
		return no_row;
	}

}
void init_keypad(void) {

	
	
	
	
}