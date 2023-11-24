#pragma once
#include <cstdint>

typedef struct 
{
    uint8_t *buffer;
    size_t   buffer_size;
    size_t   head;
    size_t   tail;
    size_t   bytes_avail;
} queue_t;

bool put(queue_t *q, uint8_t *data, size_t size);
bool get(queue_t *q, uint8_t *data, size_t size);
