#ifndef KEYPAD_H
#define KEYPAD_H

int check_rows(int cols);
int read_buttons();
char keypad_uint8_t_decode(int button_pattern);
int keypad_get_single();

#endif
