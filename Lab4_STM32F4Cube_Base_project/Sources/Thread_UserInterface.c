
/* Includes ------------------------------------------------------------------*/
#include "Thread_UserInterface.h"


/* Private variables ---------------------------------------------------------*/

// Pin maps for keypad columns and rows
const uint16_t col_pinmap[3] = {GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10};
const uint16_t row_pinmap[4] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_6, GPIO_PIN_7};

// 11 indicates '*' and 12 indicates '#'
const uint8_t keypad_map[4][3] = {
	{1, 2, 3},
	{4, 5, 6},
	{7, 8, 9},
	{11, 0, 12}};

osThreadId tid_Thread_UserInterface; 
GPIO_InitTypeDef GPIO_row, GPIO_col;
	
uint8_t key_state;
uint8_t tilt_state;   

osThreadDef(Thread_UserInterface, osPriorityBelowNormal, 1, NULL);  // TODO: See if below normal priority helps with constant calling

/* Private functions ---------------------------------------------------------*/
	
/**  Initiates user interface thread
   * @brief  Builds thread and starts it
   * @retval Integer inidicating failure or success of thread initiation
   */
int start_Thread_UserInterface (void) {

  tid_Thread_UserInterface = osThreadCreate(osThread(Thread_UserInterface ), NULL); 
  if (!tid_Thread_UserInterface){
		printf("Error starting user interface thread!");
		return(-1); 
	}
  return(0);
}

/**  User interface thread 
   * @brief  Runs UI thread which reads key presses and updates the display accordingly
   */
void Thread_UserInterface (void const *argument){
	
	while(1){
		
		osDelay(UI_THREAD_OSDELAY);
                                             
		
	}
}


/**  Configures system for user interface
   * @brief  Initializes pins for display and clock for keypad **/
void UserInterface_config(void){
	
	// Enable clocks for ports B,C,& D 
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
}

