#pragma once
#include <cstdint>

typedef struct 
{
    uint8_t flags;
    uint16_t totPacket;
    uint16_t curPacket;
    uint16_t nextPacket;
    uint32_t crc;
    uint16_t len;
    uint8_t cmd_buf[128];
}can_cmd;

void initQueueCanCmd();
void parseCanCmd();
inline void putInCanCmdBuf(uint8_t *cmd_buf, uint8_t *data, uint8_t curPacket);
void putInCmWord(uint8_t *cmd_word, uint8_t *cmd_buf, uint8_t totPacket, uint16_t len);
void printCmdWord(uint8_t *cmd_word, uint16_t len);
uint32_t calcCRCforCan(can_cmd* curCanCmd);