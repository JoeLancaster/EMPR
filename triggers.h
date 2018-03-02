#ifndef TRIGGERS_H
#define TRIGGERS_H

#include <stdint.h>

typedef enum {
  LT = 0,
  LE,
  GT,
  GE,
  EQ,
  NEQ
} trg_cond;

typedef enum {
  RED = 1,
  GREEN,
  BLUE
} trg_channel;

typedef struct {
  trg_channel channel;
  trg_cond condition;
  uint8_t val;
} trigger;

int trigger_eval(trigger *, uint8_t *);
#endif
