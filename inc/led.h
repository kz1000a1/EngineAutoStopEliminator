#ifndef _LED_H
#define _LED_H


#define LED_GREEN_Pin GPIO_PIN_1
#define LED_GREEN_Port GPIOA
#define LED_GREEN LED_GREEN_Port , LED_GREEN_Pin
#define LED_GREEN_Mode GPIO_MODE_OUTPUT_OD
#define LED_GREEN_Active_High 0

#define LED_ORANGE_Pin GPIO_PIN_0
#define LED_ORANGE_Port GPIOA
#define LED_ORANGE LED_ORANGE_Port , LED_ORANGE_Pin
#define LED_ORANGE_Mode GPIO_MODE_OUTPUT_OD
#define LED_ORANGE_Active_High 0


void led_init(void);
void led_orange_on(void);
void led_orange_off(void);
void led_green_on(void);
void led_green_off(void);

#endif
