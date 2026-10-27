#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *buffer;          // Pointer to backing byte array 
    size_t capacity;          // Total size of the backing array
    volatile size_t head;     // Write index (updated by producer/ISR)
    volatile size_t tail;     // Read index (updated by consumer)
} ringbuf_t;

// initialize a ring buffer with a caller-supplied byte array of size 'capacity'
void ringbuf_init(ringbuf_t *rb, uint8_t *buffer, size_t capacity);

// Push one byte into the buffer. Returns false if the buffer is full
bool ringbuf_push(ringbuf_t *rb, uint8_t data);

// Pop one byte from the buffer into *data. Returns false if the buffer is empty
bool ringbuf_pop(ringbuf_t *rb, uint8_t *data);

// Check if the ring buffer is currently empty or full
bool ringbuf_is_empty(const ringbuf_t *rb);
bool ringbuf_is_full(const ringbuf_t *rb);

#endif /* RINGBUF_H */