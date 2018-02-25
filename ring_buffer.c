#include "ring_buffer.h"
#include <stdlib.h>

#define rbh (rb -> head)
#define rbs (rb -> size)
#define rbt (rb -> tail)

int rb_is_empty(ring_buf_t * rb) {
  return rbh == rbt;
 
}

int rb_is_full(ring_buf_t * rb) {
  return ((rbh + 1) % rbs) == rbt;
}

void rb_de_init(ring_buf_t * rb) {
  free(rb -> buffer);
}

void rb_init(ring_buf_t * rb, size_t size) {
  rb -> buffer = malloc(size);
  rbh = 0;
  rbt = 0;
  rbs = size;
}

void rb_put(ring_buf_t * rb, uint8_t data) {
  
  rb -> buffer[rbh] = data;
  rbh = (rbh + 1) % rbs;
  if(rbh == rbt) {
    rbt = (rbt + 1) % rbs;
  }
}

uint8_t rb_get(ring_buf_t * rb) {
  uint8_t temp = 0;
  if(!rb_is_empty(rb)){
    temp = rb -> buffer[rbt];
    rbt = (rbt + 1) % rbs;
  }
  return temp;
}
