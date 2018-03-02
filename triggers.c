#include "triggers.h"
#include <stdint.h>

#define channel (t -> channel)
#define val (t -> val)
#define condition (t -> condition)

int trigger_eval(trigger * t, uint8_t * data) {
  //Data format: 4 Bytes where data[1] is red
  switch(condition){
    case LT:
      return data[channel] < val;
    case LE:
      return data[channel] <= val;
    case GT:
      return data[channel] > val;
    case GE:
      return data[channel] >= val;
    case EQ:
      return data[channel] == val;
    case NEQ:
      return data[channel] != val;
  }
}
