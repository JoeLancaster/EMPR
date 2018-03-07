#ifndef LED_H
#define LED_H

#include <stdint.h>

#define LED_0 (1 << 18)
#define LED_1 (1 << 20)
#define LED_2 (1 << 21)
#define LED_3 (1 << 23)

const int LEDs[4];

void LED_on(int);
void LED_off(int);

void all_off();

int decode(uint8_t, uint8_t);
void led_number(uint8_t);
#endif
