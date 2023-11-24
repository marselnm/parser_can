#include "circle_buffer.h"

bool put(queue_t *q, uint8_t *data, size_t size) 
{
    if(q->buffer_size - q->bytes_avail < size)
    {
        return false;
    }

    for(size_t i = 0; i < size; i++)
    {
        q->buffer[(q->tail + i) % q->buffer_size] = data[i];
    }

    q->tail = (q->tail + size) % q->buffer_size;
    q->bytes_avail += size;
    return true;
}

bool get(queue_t *q, uint8_t *data, size_t size) 
{
    if(q->bytes_avail < size)
    {
        return false;
    }
    for(size_t i = 0; i < size; i++)
    {
        data[i] = q->buffer[(q->head + i) % q->buffer_size];
    }
    q->head = (q->head + size) % q->buffer_size;
    q->bytes_avail -= size;
    return true;
}