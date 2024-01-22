#include <cstdint>
#include <cmath>
#include <iostream>

#include "parser_can.h"

queue_t queue_can_cmd;  //struct for work with circle buffer for collect can packet
static uint8_t buffer_can_cmd[BUF_CMD_SIZE];    //circle buffer for collect can packet
static can_cmd curCanCmd;       //struct for work with current can msg
static uint8_t cCmWord[150];    //cmd for control receiver

//for Rx data of chains can_id=0x27B
queue_t queue_can_chains;
uint8_t buffer_can_chains[BUF_CMD_SIZE];
can_rx curCanRxData;

/**
 * function for init struct for work with circle buffer
 * should call before can interrupt will turn on
 */
void initQueueCanCmd()
{
    queue_can_cmd.buffer = buffer_can_cmd;    
    queue_can_cmd.buffer_size = BUF_CMD_SIZE;
    queue_can_cmd.head = 0;
    queue_can_cmd.tail = 0;
    queue_can_cmd.bytes_avail = 0;

    curCanCmd.flags = eWaitHead;
    curCanCmd.totReceived = 0;
    curCanCmd.totBadCrc = 0;
}

/**
 * main function for collect can packets in one msg
 */
void parseCanCmd()
{
    uint8_t temp_buf[8];

    while (queue_can_cmd.bytes_avail >= 8)
    {
        get(&queue_can_cmd, temp_buf, 8);

        if (temp_buf[0] == 0 && temp_buf[1] == 0)
        {
            curCanCmd.flags = eParseHead;    //we found head of can msg    
        } 

        switch (curCanCmd.flags)
        {
        case eParseHead: //parse head of can msg
        {
            curCanCmd.len = (temp_buf[3] << 8) | temp_buf[2];
            curCanCmd.crc = (temp_buf[7] << 24) | (temp_buf[6] << 16) | (temp_buf[5] << 8) | temp_buf[4];
            curCanCmd.totPacket = ((curCanCmd.len + 6 - 1) / 6) + 1;
            curCanCmd.curPacket = 0;
            curCanCmd.nextPacket = 1;
            collectCanPacket(curCanCmd.cmd_buf, temp_buf, curCanCmd.curPacket);
            curCanCmd.flags = eCollectPacket;           
            break;
        }
        case eCollectPacket: //collect next packet of can msg
        {
            uint16_t packet = (temp_buf[1] << 8) | temp_buf[0];

            if (packet == curCanCmd.nextPacket)
            {
                curCanCmd.curPacket++;
                curCanCmd.nextPacket++;
                collectCanPacket(curCanCmd.cmd_buf, temp_buf, curCanCmd.curPacket);

                if (curCanCmd.nextPacket == curCanCmd.totPacket)
                {
                    curCanCmd.flags = eMsgReceieved;    //we collected all packet in can msg
                }
            }
            else
            {
                curCanCmd.flags = eWaitHead;    //reset because we don't have correct num of paket in can msg    
            }

            if (curCanCmd.flags == eMsgReceieved)   //we have full can msg
            {
                uint32_t crc_in = (curCanCmd.cmd_buf[7] << 24) | (curCanCmd.cmd_buf[6] << 16) | (curCanCmd.cmd_buf[5] << 8) | curCanCmd.cmd_buf[4];

                uint32_t crc_calc = calcCRCforCan(&curCanCmd);

                if (crc_calc == crc_in) //check crc
                {
                    convertInReceiverMsg(cCmWord, curCanCmd.cmd_buf, curCanCmd.totPacket, curCanCmd.len);    //call put in CmWord
                    curCanCmd.totReceived++;
                    printCmdWord(cCmWord, curCanCmd.len);//call identifier
                    curCanCmd.flags = eWaitHead;    //for new cmd
                }
                else
                {
                    curCanCmd.totBadCrc++;
                    curCanCmd.flags = eWaitHead;    //reset because we don't have correct crc
                    printf("bad crc %d\n", curCanCmd.totBadCrc); 
                }
            } 
            break;
        }

        default:
            break;
        }

    }
}

/**
 * local function for copy current packet in can msg buffer
 */
void collectCanPacket(uint8_t *cmd_buf, uint8_t *data, uint8_t curPacket)
{
    for(int i = 0; i < 8; ++i)
    {
        cmd_buf[8*curPacket + i] = data[i]; 
    } 
}

/**
 * function for convert in receiver msg
 */
void convertInReceiverMsg(uint8_t *cmd_word, uint8_t *cmd_buf, uint8_t totPacket, uint16_t len)
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

/**
 * calculation CRC for can msg
 */
uint32_t calcCRCforCan(can_cmd* curCanCmd)
{
    uint16_t remder = curCanCmd->len % 6; 
    uint16_t calc_len = (remder) ? (8 * curCanCmd->totPacket  - (6 - remder)) : (8 * curCanCmd->totPacket);

    uint32_t crc = 0;

    for (int i = 8; i < calc_len; ++i)
    {
       if ((i % 8 == 0) || ((i - 1) % 8 == 0))
       {
            continue;
       }

       crc += curCanCmd->cmd_buf[i]; 
    }

    return crc;
}

/**
 * print received msg, for debug only
 */
void printCmdWord(uint8_t *cmd_word, uint16_t len)
{
    printf("%d ", (int)curCanCmd.totReceived);

    for (int i = 0; i < len + 3; ++i)
    {
        printf("%c", cmd_word[i]);
    }
    printf("\n");
}

/**
 * function for init struct for work with circle buffer
 * should call before can interrupt will turn on
 */
void initQueueRxCan(queue_t* queue_rx_can, uint8_t* buffer, can_rx* curCanRx)
{
    queue_rx_can->buffer = buffer;    
    queue_rx_can->buffer_size = BUF_CMD_SIZE;
    queue_rx_can->head = 0;
    queue_rx_can->tail = 0;
    queue_rx_can->bytes_avail = 0;

    curCanRx->flags = eWaitHead;
    curCanRx->totReceived = 0;
    curCanRx->totBadCrc = 0;
}

int parseCanRx(queue_t* queue_rx_can, can_rx* cur_can_rx, stNewChain* chain_can)
{
    uint8_t temp_buf[8];

    while (queue_rx_can->bytes_avail >= 8)
    {
        get(queue_rx_can, temp_buf, 8);

        if (temp_buf[0] == 0 && temp_buf[1] == 0)
        {
            cur_can_rx->flags = eParseHead;    //we found head of can msg    
        } 

        switch (cur_can_rx->flags)
        {
        case eParseHead: //parse head of can msg
        {
            cur_can_rx->len = (temp_buf[3] << 8) | temp_buf[2];
            cur_can_rx->crc = (temp_buf[7] << 24) | (temp_buf[6] << 16) | (temp_buf[5] << 8) | temp_buf[4];
            cur_can_rx->totPacket = ((cur_can_rx->len + 6 - 1) / 6) + 1;
            cur_can_rx->curPacket = 0;
            cur_can_rx->nextPacket = 1;
            collectCanPacket(cur_can_rx->buf, temp_buf, cur_can_rx->curPacket);
            cur_can_rx->flags = eCollectPacket;           
            break;
        }
        case eCollectPacket: //collect next packet of can msg
        {
            uint16_t packet = (temp_buf[1] << 8) | temp_buf[0];

            if (packet == cur_can_rx->nextPacket)
            {
                cur_can_rx->curPacket++;
                cur_can_rx->nextPacket++;
                collectCanPacket(cur_can_rx->buf, temp_buf, cur_can_rx->curPacket);

                if (cur_can_rx->nextPacket == cur_can_rx->totPacket)
                {
                    cur_can_rx->flags = eMsgReceieved;    //we collected all packet in can msg
                }
            }
            else
            {
                cur_can_rx->flags = eWaitHead;    //reset because we don't have correct num of paket in can msg    
            }

            if (cur_can_rx->flags == eMsgReceieved)   //we have full can msg
            {
                uint32_t crc_in = (cur_can_rx->buf[7] << 24) | (cur_can_rx->buf[6] << 16) | (cur_can_rx->buf[5] << 8) | cur_can_rx->buf[4];

                uint32_t crc_calc = calcCRCRxCan(cur_can_rx);

                if (crc_calc == crc_in) //check crc
                {
                    writeDataInStruct(cur_can_rx, chain_can);
                    cur_can_rx->totReceived++;
                    curCanCmd.flags = eWaitHead;    //for new cmd
                    return 1;
                }
                else
                {
                    cur_can_rx->totBadCrc++;
                    cur_can_rx->flags = eWaitHead;    //reset because we don't have correct crc
                    printf("bad crc %d\n", cur_can_rx->totBadCrc); 
                }
            } 
            break;
        }

        default:
            break;
        }

    }

    return 0;
}

uint32_t calcCRCRxCan(can_rx* cur_can_rx)
{
    uint16_t remder = cur_can_rx->len % 6; 
    uint16_t calc_len = (remder) ? (8 * cur_can_rx->totPacket  - (6 - remder)) : (8 * cur_can_rx->totPacket);

    uint32_t crc = 0;

    for (int i = 8; i < calc_len; ++i)
    {
       if ((i % 8 == 0) || ((i - 1) % 8 == 0))
       {
            continue;
       }

       crc += cur_can_rx->buf[i]; 
    }

    return crc;
}

void writeDataInStruct(can_rx* cur_can_rx, stNewChain* chain_can)
{
    uint8_t* p_st = (uint8_t*)(chain_can);

    for (int i = 1; i < cur_can_rx->totPacket; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            p_st[6*(i-1) + j] = cur_can_rx->buf[10 + 8 * (i - 1) + j]; 
        }
    }
}