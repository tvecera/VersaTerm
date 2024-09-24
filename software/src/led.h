
#ifndef _VERSATERM_LED_H
#define _VERSATERM_LED_H

// LED1 index
#ifndef LED1
#define LED1 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

// set LED ON (inx = LED index LED?_INX)
void led_on(unsigned char inx);

// set LED OFF (inx = LED index LED?_INX)
void led_off(unsigned char inx);

// set LED (inx = LED index LED?)
void led_set(unsigned char inx, unsigned char val);

// flip LED (inx = LED index LED?_INX)
void led_flip(unsigned char inx);

// initialize LEDs
void led_init();

#ifdef __cplusplus
}
#endif

#endif //_VERSATERM_LED_H
