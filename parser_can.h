#pragma once
#include <cstdint>
#include "circle_buffer.h"
#include "struct.h"

#define BUF_CMD_SIZE 512

enum
{
    eWaitHead = 0,      //wait head of can msg, which contain id, len of cmd and crc
    eParseHead = 1,     //recieved head and can parse it
    eCollectPacket = 2, //for collect packet of can msg
    eMsgReceieved = 3   //we receieved can msg
};

/**
 * struct for work with current can msg
 */
typedef struct 
{
    uint8_t flags;          //for machine state
    uint16_t totPacket;     //number of packet in current can msg
    uint16_t curPacket;     //number of current can packet
    uint16_t nextPacket;    //number of next can packet
    uint32_t crc;           //CRC for can msg (can find this in first can packet)
    uint16_t len;           //len of data in one can msg (can find this in first can packet)
    uint8_t cmd_buf[128];   //local buffer for current can msg
    
    uint32_t totReceived;   //for all msg, count received msg, clean after reset board
    uint32_t totBadCrc;     //for all msg, count msg with bad CRC, clean after reset board 
}can_cmd;

typedef struct 
{
    uint8_t flags;          //for machine state
    uint16_t totPacket;     //number of packet in current can msg
    uint16_t curPacket;     //number of current can packet
    uint16_t nextPacket;    //number of next can packet
    uint32_t crc;           //CRC for can msg (can find this in first can packet)
    uint16_t len;           //len of data in one can msg (can find this in first can packet)
    uint8_t buf[200];       //local buffer for current can msg
    
    uint32_t totReceived;   //for all msg, count received msg, clean after reset board
    uint32_t totBadCrc;     //for all msg, count msg with bad CRC, clean after reset board 
}can_rx;


void initQueueCanCmd();
void parseCanCmd();
void collectCanPacket(uint8_t *cmd_buf, uint8_t *data, uint8_t curPacket);
void convertInReceiverMsg(uint8_t *cmd_word, uint8_t *cmd_buf, uint8_t totPacket, uint16_t len);
void printCmdWord(uint8_t *cmd_word, uint16_t len);
uint32_t calcCRCforCan(can_cmd* curCanCmd);

void initQueueRxCan(queue_t* queue_rx_can, uint8_t* buffer, can_rx* curCanRx);
int parseCanRx(queue_t* queue_rx_can, can_rx* cur_can_rx, stNewChain* chain_can);
uint32_t calcCRCRxCan(can_rx* cur_can_rx);
void writeDataInStruct(can_rx* cur_can_rx, stNewChain* chain_can);

