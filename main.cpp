#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "parser_can_dump.h"
#include "circle_buffer.h"
#include "parser_can.h"

extern queue_t queue_can_chains;
extern uint8_t buffer_can_chains[BUF_CMD_SIZE];
extern can_rx curCanRxData;

using namespace std;

int main()
{
    std::cout << "Hello from parser can" << std::endl;

    std::ifstream fCanDump;
    fCanDump.open("can_chains_0x27B.txt");

    vector<canPacket> vCanID27B;
    putCmdFromCanDump(fCanDump, vCanID27B, "0x27B STD Rx 8 ");

    initQueueRxCan(&queue_can_chains, buffer_can_chains, &curCanRxData);

    for (int i = 0; i < vCanID27B.size(); ++i)
    {
        uint8_t temp[8];

        for(int j = 0; j < 8; ++j)
        {
            temp[j] = vCanID27B.at(i).bytes_[j];
        }

        put(&queue_can_chains, temp, 8);
    }

    stNewChain chainCurCan;
    for (int i = 0; i < 500; ++i)
    {
        if (parseCanRx(&queue_can_chains, &curCanRxData, &chainCurCan))
        {
            std::cout << "new chain:" << std::endl;
            printf("%d\n", chainCurCan.nWorkMode);
            printf("%d\n", chainCurCan.Cp);

            for (int i = 0; i < 5; ++i)
            {
                printf("%d ", chainCurCan.fB[i]);
            }
            printf("\n");

            for (int i = 0; i < 5; ++i)
            {
                printf("%d ", chainCurCan.fL[i]);
            }
            printf("\n");

            for (int i = 0; i < 5; ++i)
            {
                printf("%d ", chainCurCan.fED[i]);
            }
            printf("\n");

            printf("%s", chainCurCan.mName_GRI);
            printf("\n");
        }
    }
    


    return 0;
}

