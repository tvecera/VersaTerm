#ifndef _VERSATERM_KEY_H
#define _VERSATERM_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

// buttons codes (= button indice + 1) (0 = no key)
#ifndef KEY_DEFAULTS
#define KEY_DEFAULTS 1
#endif

#ifndef SYSTICK_KEYSCAN
#define SYSTICK_KEYSCAN 1
#endif

#ifndef KEY_REP_TIME1
#define KEY_REP_TIME1 400  // delta time of first press in [ms] (max 500)
#endif

#ifndef KEY_REP_TIME2
#define KEY_REP_TIME2 200  // delta time of repeat press in [ms] (max 500)
#endif

// timings
#ifndef KEY_REL_TIME
#define KEY_REL_TIME 50  // delta time of release in [ms]
#endif

#ifndef SYSTICK_KEYSCAN_DELAY
#define SYSTICK_KEYSCAN_DELAY 50000  // the repeat delay in microseconds
#endif

#define NOKEY 0  // no key from keyboard

// initialize keys
void key_init();

// check if button KEY_* is currently pressed
bool key_pressed(char key);

// get button from keyboard buffer (returns NOKEY if no scan code)
char key_get();

// flush keyboard buffer
void key_flush();

// return key to keyboard buffer (can hold only 1 key)
void key_ret(char key);

// check no pressed key
bool key_no_pressed();

// wait for no key pressed
void key_wait_no_pressed();

#ifdef __cplusplus
}
#endif

#endif //_VERSATERM_KEY_H
