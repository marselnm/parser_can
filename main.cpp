#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "parser_can_dump.h"
#include "circle_buffer.h"
#include "parser_can.h"

extern queue_t queue_can_cmd;

using namespace std;

int main()
{
    std::cout << "Hello from parser can" << std::endl;

    std::ifstream fCanDump;

    vector<canPacket> vCanID222;

    //fCanDump.open("dump_can.txt");
    fCanDump.open("can_cmd_debug_0x222.txt");

    putCmdFromCanDump(fCanDump, vCanID222);

    initQueueCanCmd();

    for (int i = 0; i < vCanID222.size(); ++i)
    {
        uint8_t temp[8];

        for(int j = 0; j < 8; ++j)
        {
            temp[j] = vCanID222.at(i).bytes_[j];
        }

        put(&queue_can_cmd, temp, 8);
    }


    parseCanCmd();


    return 0;
}

