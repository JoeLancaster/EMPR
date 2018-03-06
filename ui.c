#include "ui.h"
#include "helpers.h"

#include <stdint.h>
#include <stdlib.h>


int integer_input() {
  int state = 0xFF;
  uint8_t str[16];
  uint8_t ptr = 0;
  uint8_t v;
  lcd_init();
  do {
    while(read_buttons() == 0xFF);
    state = read_buttons();
    while(read_buttons() != 0xFF);
    v = keypad_uint8_t_decode(state);
    if((v < '0' || v > '9') && v != 'D'){
      //error
    }
    else if (v == 'D'){
      if(ptr >= 1){
      write_usb_serial_blocking("D\n\r", 3);
      str[ptr--] = ' ';
      lcd_init();
      lcd_write_str(str, 0, 0, ptr + 1);}
    }
    else {
      str[ptr++] = v;
      lcd_init();
      lcd_write_str(str, 0, 0, ptr + 1);
    }
  } while(keypad_uint8_t_decode(state) != '#');
  return atoi(str);
}
