#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

#include "parser_can_dump.h"
#include "circle_buffer.h"
#include "parser_can.h"

extern queue_t queue_can_cmd;

using namespace std;

void waitingData();

int main()
{
    std::cout << "Hello from parser can" << std::endl;

    std::ifstream fCanDump;

    vector<canPacket> vCanID222;

    //fCanDump.open("dump_can.txt");
    fCanDump.open("can_cmd_debug_0x222.txt");

    putCmdFromCanDump(fCanDump, vCanID222);

    initQueueCanCmd();

    thread th_parser(waitingData);
    th_parser.detach();

    for (int i = 0; i < vCanID222.size(); ++i)
    {
        uint8_t temp[8];

        for(int j = 0; j < 8; ++j)
        {
            temp[j] = vCanID222.at(i).bytes_[j];
        }

        this_thread::sleep_for(chrono::milliseconds(250));
        put(&queue_can_cmd, temp, 8);
        cout << "put packet\t" << "Bytes in queue: " << queue_can_cmd.bytes_avail << endl;
    }

    this_thread::sleep_for(chrono::milliseconds(5000));//for wait parser
    return 0;
}

void waitingData()
{
    for(;;)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
        parseCanCmd();
    }
}

