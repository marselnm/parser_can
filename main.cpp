#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "parser_can_dump.h"

using namespace std;


typedef struct 
{
    uint8_t *buffer;
    size_t   buffer_size;
    size_t   head;
    size_t   tail;
    size_t   bytes_avail;
} queue_t;

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

int main()
{
    uint8_t FIFO[128];

    queue_t queue;
    queue.buffer   = FIFO;
    queue.buffer_size = 50;
    queue.head        = 0;
    queue.tail        = 0;
    queue.bytes_avail = 0;


    uint8_t data[8] = {1,2,3,4,5,8,9,10};

    put(&queue, data, 8); 
    put(&queue, data, 8); 
    put(&queue, data, 8); 
    put(&queue, data, 8); 
    put(&queue, data, 8); 
    put(&queue, data, 8); 

    get(&queue, data, 8);
    get(&queue, data, 8);
    get(&queue, data, 8);
    put(&queue, data, 8); 


    return 0;
}

