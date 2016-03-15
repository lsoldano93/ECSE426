

/* Includes ------------------------------------------------------------------*/
#include "Thread_SegmentDisplay.h"

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**  Initializes GPIO pins for 7 segment display
   * @brief  Initializes pins for display **/
void init_display(void){
	
	// WARNING: DO NOT USE PORT A13, PORT A14, or PORT B3
	/* 7-Segment Display Pinout {1:CCD1, 2:CCD2, 3:D, 4:Degree, 5:E, 6:CCD3, 
	   7:DP, 8:CCD4, 9:, 10:, 11:F, 12:, 13:C, 14:A, 15:G, 16:B}
	
		 Keypad Pinout {1:R1, 2:R2, 4:R4, 5:R5, 6:C1, 7:C2, 8:C3}
	
	   GPIO Pin Mapping for Display
		 Segment Ctrls {CCD1:PC1, CCD2:PC2, CCD3:PC4, CCD4:PC5}
	   DP & DC Ctrl  {DP:PC6, DC:PC7}
		 Segments      {A:PB0, B:PB1, C:PB5, D:PB11, E:PB12, F:PB13, G:PB14} 
	
		 GPIO Pin Mapping for Keypad
		 Rows: {R1:D1, R2:D2, R3:D6 , R4:D7}
		 Columns: {C1:D8, C2:D9, C3:D10}
	
		 Alarm LED : PD13
	*/
	
	GPIO_InitTypeDef GPIO_InitB, GPIO_InitC;
	
	// Enable clocks for ports B & C 
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	// Give initialization values for GPIO B pin sets
	GPIO_InitB.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_InitB.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitB.Pull = GPIO_PULLUP;
	GPIO_InitB.Speed =  GPIO_SPEED_FREQ_HIGH;
	
	// Give initialization values for GPIO C pin sets
	GPIO_InitC.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitC.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitC.Pull = GPIO_PULLUP;
	GPIO_InitC.Speed =  GPIO_SPEED_FREQ_HIGH;

	// Initialize all GPIO pin sets
	HAL_GPIO_Init(GPIOB, &GPIO_InitB);
	HAL_GPIO_Init(GPIOC, &GPIO_InitC);
	
}


/**  Selects segment digit to display for
   * @brief  Cycles through cases for each segment of display and enables/disables proper pins
	 * @param  Segment digit to be illuminated **/
void selectDigit(int digit) {
	
	switch(digit) {
		case 4:
			// Enable changes for first digit display
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);   // Digit 1
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET); // Digit 2
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET); // Digit 3
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET); // Digit 4
			break;
		case 3:
			// Enable changes for second digit display
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); // Digit 1
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);   // Digit 2
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET); // Digit 3
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET); // Digit 4
			break;
		case 2:
			// Enable changes for third digit display
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); // Digit 1
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET); // Digit 2
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);   // Digit 3
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET); // Digit 4
			break;
		case 1:
			// Enable changes for fourth digit display
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); // Digit 1
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET); // Digit 2
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET); // Digit 3
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);   // Digit 4
			break;

		default:
			break;
	} 
}


/**  Lights segment digit selected with proper display
   * @brief  Takes in value to be displayed on preselected 7 segment display
	 * @param  Value to be displayed **/
void lightNum(int num) {
	
	/* 7-Segment Display Pinout {1:CCD1, 2:CCD2, 3:D, 4:Degree, 5:E, 6:CCD3, 
	   7:DP, 8:CCD4, 9:, 10:, 11:F, 12:, 13:C, 14:A, 15:G, 16:B}
	
		 Keypad Pinout {1:R1, 2:R2, 4:R4, 5:R5, 6:C1, 7:C2, 8:C3}
	
	   GPIO Pin Mapping for Display
		 Segment Ctrls {CCD1:PC1, CCD2:PC2, CCD3:PC4, CCD4:PC5}
	   DP & DC Ctrl  {DP:PC6, DC:PC7}
		 Segments      {A:PB0, B:PB1, C:PB5, D:PB11, E:PB12, F:PB13, G:PB14} 
	
		 GPIO Pin Mapping for Keypad
		 Rows: {R1:PD1, R2:PD2, R3:PD6 , R4:PD7}
		 Columns: {C1:PD8, C2:PD9, C3:PD10}
	
		 Alarm LED : PD13
	*/
	
	switch(num) {
			case 0:
				// Zeroth state, display number zero
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //g
				break;
			
			case 1:
				// First state, display number one
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //g
				break;

			case 2:
				// Second state, display number two
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g
				break;
			
			case 3:
				// Third state, display number three
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g
				break;
			
			case 4:
				// Fourth state, display number four
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g			
				break;
			
			case 5:
				// Fifth state, display number five
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g
				break;
			
			case 6:
				// Sixth state, display number six
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g
				break;

			case 7:
				// Seventh state, display number seven
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //g
				break;
			
			case 8:
				// Eighth state, display number eight
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g
				break;
			
			case 9:
				// Ninth state, display number nine
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g			
				break;
			
			case 10:
				// Null state, display blank digit
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //g		
				break;
				
			default:
				break;
		}
	
}


/**  Causes the display of a temperature value
   * @brief  Draws temperature on display
	 * @param  Float of temperature to be displayed **/
void draw_temperature(float temp){
	
	/* 7-Segment Display Pinout {1:CCD1, 2:CCD2, 3:D, 4:Degree, 5:E, 6:CCD3, 
	   7:DP, 8:CCD4, 9:, 10:, 11:F, 12:, 13:C, 14:A, 15:G, 16:B}
	
		 Keypad Pinout {1:R1, 2:R2, 4:R4, 5:R5, 6:C1, 7:C2, 8:C3}
	
	   GPIO Pin Mapping for Display
		 Segment Ctrls {CCD1:PC1, CCD2:PC2, CCD3:PC4, CCD4:PC5}
	   DP & DC Ctrl  {DP:PC6, DC:PC7}
		 Segments      {A:PB0, B:PB1, C:PB5, D:PB11, E:PB12, F:PB13, G:PB14} 
	
		 GPIO Pin Mapping for Keypad
		 Rows: {R1:PD1, R2:PD2, R3:PD6 , R4:PD7}
		 Columns: {C1:PD8, C2:PD9, C3:PD10}
	
		 Alarm LED : PD13
	*/
	
	int displayValues, i;
	int tempValue[4];
	
	// Determine how many values to display and what each individual value is
	// Four digits to display (format xyz.a)
	if (temp >= 100.0){
		displayValues = 4;
		tempValue[0] = temp/100;
		tempValue[1] = temp/10 - tempValue[0]*10;
		tempValue[2] = temp - tempValue[0]*100 - tempValue[1]*10;
		tempValue[3] = (temp*10 - tempValue[0]*1000 - tempValue[1]*100 - tempValue[2]*10);
	
	}
	else {
		
		// Three digits to display (format xy.z)
		if (temp >= 10.0){
				displayValues = 3;
				tempValue[1] = temp/10;
				tempValue[2] = temp - tempValue[1]*10;
				tempValue[3] = (temp*10 - tempValue[1]*100 - tempValue[2]*10);	
		}
		
		// Two digits to display (format x.y or 0.x)
		else {
				displayValues = 2;
				tempValue[2] = temp;
				tempValue[3] = (temp*10 - tempValue[2]*10);
		}
	}
	
	for(i=4; i>0; i--) {
		
		selectDigit(i);
		reset();
		 
		// Light proper number on display, lighting 10 indicates blank display value
		if (i > displayValues) {
			lightNum(10);
		}
		else {
			// Set decimal point on third value
			if (i == 2) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			
			lightNum(tempValue[4-i]);	
			
		}
	}
		
		osDelay(1);
}	


/**  Causes the display of an angle value
   * @brief  Draws angle on display
	 * @param  Float of angle to be displayed **/
void draw_angle(float angle) {
	
	/* 7-Segment Display Pinout {1:CCD1, 2:CCD2, 3:D, 4:Degree, 5:E, 6:CCD3, 
	   7:DP, 8:CCD4, 9:, 10:, 11:F, 12:, 13:C, 14:A, 15:G, 16:B}
	
		 Keypad Pinout {1:R1, 2:R2, 4:R4, 5:R5, 6:C1, 7:C2, 8:C3}
	
	   GPIO Pin Mapping for Display
		 Segment Ctrls {CCD1:PC1, CCD2:PC2, CCD3:PC4, CCD4:PC5}
	   DP & DC Ctrl  {DP:PC6, DC:PC7}
		 Segments      {A:PB0, B:PB1, C:PB5, D:PB11, E:PB12, F:PB13, G:PB14} 
	
		 GPIO Pin Mapping for Keypad
		 Rows: {R1:PD1, R2:PD2, R3:PD6 , R4:PD7}
		 Columns: {C1:PD8, C2:PD9, C3:PD10}
	
		 Alarm LED : PD13
	*/
	
	int i, decimalPos;
	int displayValues = 3;
	int angleValue[3];
	
	//7-segment display in the form of XXX°, XX.Y °or X.YY depending on the actual angle(i.e. 119°, 59.3°, 7.55°)
	//XXX°
	if (angle >= 100.0){
		
		// Don't want to light up decimal in this case, give dummy value
		decimalPos = 30;
		angleValue[0] = angle/100;
		angleValue[1] = angle/10 - angleValue[0]*10;
		angleValue[2] = angle - angleValue[0]*100 - angleValue[1]*10;

	}
	
	// Three digits to display (XX.Y°)
	else if(angle >= 10.0) {

		decimalPos = 2;
		angleValue[0] = angle/10;
		angleValue[1] = angle - angleValue[0]*10;
		angleValue[2] = (angle*10 - angleValue[0]*100 - angleValue[1]*10);	
	
	}
	
	//Two digits to display (X.YY°)
	else {
		decimalPos = 3;
		angleValue[0] = angle;
		angleValue[1] = (angle*10 - angleValue[0]*10);
		angleValue[2] = (angle*100 - angleValue[0]*100 - angleValue[1]*10);

	}	
	
	for(i=4; i>0; i--) {
		
		// For display of degree symbol
		if (i ==4 ) {
			
			reset();
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			
		}
		else {
			
			reset();
			selectDigit(i);
			if (i == decimalPos) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);	

			lightNum(angleValue[3-i]);	
		
			
		}
			
		// Interrupts at 500 HZ, delay of 4 ms
		osDelay(2);
		
	}
}


/**  Resets all display pins
   * @brief  "" **/
void reset() {
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); //a
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); //b
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); //c
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); //d
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //f
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //g
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);   // Digit 1
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET); // Digit 2
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET); // Digit 3
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET); // Digit 4
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET); //degree
	
}
