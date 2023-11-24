#include "parser_can.h"
#include <cstdint>

#include "circle_buffer.h"

#define BUF_CMD_SIZE 128
static uint8_t buffer_can_cmd[BUF_CMD_SIZE];
queue_t queue_can_cmd;

void initQueueCanCmd()
{
    queue_can_cmd.buffer = buffer_can_cmd;    
    queue_can_cmd.buffer_size = BUF_CMD_SIZE;
    queue_can_cmd.head = 0;
    queue_can_cmd.tail = 0;
    queue_can_cmd.bytes_avail = 0;
}

void parseCanCmd()
{
    
}