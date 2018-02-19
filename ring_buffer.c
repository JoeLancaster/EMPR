#include "ring_buffer.h"
#include <stdlib.h>

int rb_is_empty(ring_buf_t * rb) {
  return (rb -> head) == (rb -> tail);
}

int rb_is_full(ring_buf_t * rb) {
  return ((rb -> head + 1) % rb -> size) == rb -> tail;
}

void rb_de_init(ring_buf_t * rb) {
  free(rb -> buffer);
}

void rb_init(ring_buf_t * rb, size_t size) {
  rb -> buffer = malloc(size);
  rb -> head = 0;
  rb -> tail = 0;
  rb -> size = size;
}

void rb_put(ring_buf_t * rb, uint8_t data) {
  rb -> buffer[rb -> head] = data;
  rb -> head = (rb -> tail + 1) % rb -> size;
  if(rb_is_empty(rb)) {
    rb -> tail = (rb -> tail + 1) % rb -> size;
  }
}

uint8_t rb_get(ring_buf_t * rb) {
  uint8_t temp = 22;
  if(!rb_is_empty(rb)){
    temp = rb -> buffer[rb -> tail];
    rb -> tail = (rb -> tail + 1) % rb -> size;
  }
  return temp;
}
