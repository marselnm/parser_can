#pragma once
#include <cstdint>

void initQueueCanCmd();
void parseCanCmd();
void putInCanCmdBuf(uint8_t *cmd_buf, uint8_t *data, uint8_t curPacket);
void putInCmWord(uint8_t *cmd_word, uint8_t *cmd_buf, uint8_t totPacket, uint16_t len);
void Idenifier(uint8_t *cmd_word, uint16_t len);