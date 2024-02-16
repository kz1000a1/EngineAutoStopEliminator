//
// LED: Handles blinking of status light
//

#include "stm32f0xx_hal.h"
#include "led.h"



// Initialize LED GPIOs
void led_init()
{
   
    GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = LED_GREEN_Pin;
    GPIO_InitStruct.Mode = LED_GREEN_Mode;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(LED_GREEN_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED_ORANGE_Pin;
    GPIO_InitStruct.Mode = LED_ORANGE_Mode;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(LED_ORANGE_Port, &GPIO_InitStruct);
   
    led_green_off(); 
    led_orange_off();  
}


// Turn orange LED on
void led_orange_on(void)
{
	HAL_GPIO_WritePin(LED_ORANGE, LED_ORANGE_Active_High);
}


// Turn orange LED on
void led_orange_off(void)
{
	HAL_GPIO_WritePin(LED_ORANGE, ~ LED_ORANGE_Active_High);
}


// Turn green LED on
void led_green_on(void)
{
	HAL_GPIO_WritePin(LED_GREEN, LED_GREEN_Active_High);
}


// Turn green LED of
void led_green_off(void)
{
	HAL_GPIO_WritePin(LED_GREEN, ~ LED_GREEN_Active_High);
}
