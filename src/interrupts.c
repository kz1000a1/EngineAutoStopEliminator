//
// interrupts: handle global system interrupts
//

#include "stm32f0xx_hal.h"
#include "interrupts.h"
#include "can.h"



// Externs
extern PCD_HandleTypeDef hpcd_USB_FS;



void NMI_Handler(void)
{
	while(1);
}

void HardFault_Handler(void)
{
	while(1);
}


// Handle USB interrupts
void USB_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_FS);
}


// Handle SysTick interrupt
void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}


// Handle CAN interrupts
void CEC_CAN_IRQHandler(void)
{
    HAL_CAN_IRQHandler(can_gethandle());
}
