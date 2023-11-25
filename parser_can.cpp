#include "parser_can.h"
#include <cstdint>
#include <cmath>
#include <iostream>

#include "circle_buffer.h"

#define BUF_CMD_SIZE 6000
static uint8_t buffer_can_cmd[BUF_CMD_SIZE];
queue_t queue_can_cmd;

static uint8_t 	cCmWord[150];	// команда для управления платой
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

static can_cmd curCanCmd;

void initQueueCanCmd()
{
    queue_can_cmd.buffer = buffer_can_cmd;    
    queue_can_cmd.buffer_size = BUF_CMD_SIZE;
    queue_can_cmd.head = 0;
    queue_can_cmd.tail = 0;
    queue_can_cmd.bytes_avail = 0;

    curCanCmd.flags = 0;
}


// 09:06:59.906 2 0x222 STD Rx 8 00 00 0B 00 81 02 00 00

// 09:06:59.906 2 0x222 STD Rx 8 01 00 24 49 46 43 4F 52

// 09:06:59.906 2 0x222 STD Rx 8 02 00 31 30 31 32 26 32


void parseCanCmd()
{
    uint8_t temp_buf[8];

    while (queue_can_cmd.bytes_avail >= 8)
    {
        get(&queue_can_cmd, temp_buf, 8);

        if (temp_buf[0] == 0 && temp_buf[1] == 0)
        {
            curCanCmd.flags = 1;    //we found head of can msg    
        } 

        switch (curCanCmd.flags)
        {
        case 1: //parse head of can msg
        {
            curCanCmd.len = (temp_buf[3] << 8) | temp_buf[2];
            curCanCmd.crc = (temp_buf[7] << 24) | (temp_buf[6] << 16) | (temp_buf[5] << 8) | temp_buf[4];
            curCanCmd.totPacket = ((curCanCmd.len + 6 - 1) / 6) + 1;
            curCanCmd.curPacket = 0;
            curCanCmd.nextPacket = 1;
            putInCanCmdBuf(curCanCmd.cmd_buf, temp_buf, curCanCmd.curPacket);
            curCanCmd.flags = 2;           
            break;
        }
        case 2: //collect next packet of can msg
        {
            uint16_t packet = (temp_buf[1] << 8) | temp_buf[0];

            if (packet == curCanCmd.nextPacket)
            {
                curCanCmd.curPacket++;
                curCanCmd.nextPacket++;
                putInCanCmdBuf(curCanCmd.cmd_buf, temp_buf, curCanCmd.curPacket);

                if (curCanCmd.nextPacket == curCanCmd.totPacket)
                {
                    curCanCmd.flags = 3;    //we collected all packet in can msg
                }
            }
            else
            {
                curCanCmd.flags = 0;    //reset because we don't have correct num of paket in can msg    
            }

            if (curCanCmd.flags == 3)   //we have full can msg
            {
                if (1)//this we should check crc
                {
                    putInCmWord(cCmWord, curCanCmd.cmd_buf, curCanCmd.totPacket, curCanCmd.len);//call put in CmWord
                    Idenifier(cCmWord, curCanCmd.len);//call identifier
                    curCanCmd.flags = 0; //for new cmd
                }
                else
                {
                    curCanCmd.flags = 0;
                }
            } 
            break;
        }

        default:
            break;
        }

    }
}

void putInCanCmdBuf(uint8_t *cmd_buf, uint8_t *data, uint8_t curPacket)
{
    for(int i = 0; i < 8; ++i)
    {
        cmd_buf[8*curPacket + i] = data[i]; 
    } 
}

void putInCmWord(uint8_t *cmd_word, uint8_t *cmd_buf, uint8_t totPacket, uint16_t len)
{
    for (int i = 1; i < totPacket; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            cmd_word[6*(i-1) + j] = cmd_buf[10 + 8 * (i - 1) + j]; 
        }
    }

    cmd_word[len - 1] = 'e';
    cmd_word[len] = 'n';
    cmd_word[len + 1] = 'd';
    cmd_word[len + 2] = '&';
}

void Idenifier(uint8_t *cmd_word, uint16_t len)
{
    for (int i = 0; i < len + 3; ++i)
    {
        printf("%c", cmd_word[i]);
    }
    printf("\n");
}