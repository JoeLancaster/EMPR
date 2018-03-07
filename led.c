#include "lpc17xx_gpio.h"
#include "led.h"
#include <stdint.h>

#define LED_CNT 4

/*
  static/extern?
 */

const int LEDs[4] = {LED_0, LED_1, LED_2, LED_3};

void LED_on(int LED_n){
  GPIO_SetDir(1, LED_n, 1);
  GPIO_SetValue(1, LED_n);
}
/*
  Can this be a macro?
 */
void LED_off(int LED_n){
  GPIO_ClearValue(1, LED_n);
}

void all_off(){
  uint8_t i;
  for(i = 0; i < LED_CNT; i++){
    LED_off(LEDs[i]);
  }
}

int decode(uint8_t num, uint8_t len){
  int result, cnt;
  uint8_t mask = 1;
  /*
    Checks each column of a binary number from 2^0 to 2^len and binary ORs the
    LED value that corresponds to that column onto result
   */
  for(cnt = 0, result = 0; cnt < len; cnt++, mask = mask << 1){
    result |= ((num & mask) == mask ? LEDs[cnt] : 0);
  }
  return result;
}

/*
  Can this be a macro?
 */
void led_number(uint8_t num){
  LED_on(decode(num, LED_CNT));
}
