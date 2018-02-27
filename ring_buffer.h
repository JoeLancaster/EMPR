#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
  uint8_t * buffer;
  size_t head;
  size_t tail;
  size_t size;
} ring_buf_t;

void rb_init(ring_buf_t *, size_t);
void rb_de_init(ring_buf_t *);
int rb_is_empty(ring_buf_t *);
void rb_put(ring_buf_t *, uint8_t);
uint8_t rb_get(ring_buf_t *);

#endif
