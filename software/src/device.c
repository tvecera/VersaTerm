#include "pins.h"

#if !USE_DEVICE_INIT

#include "device.h"
#include "key.h"
#include "led.h"

void __in_flash(".device") device_init() {
	key_init();
	led_init();
}

void __in_flash(".device") device_started() { }

#endif
