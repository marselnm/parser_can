#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "parser_can_dump.h"

using namespace std;

int main()
{
    std::cout << " Hello for parser can" << std::endl;

    std::ifstream fCanDump;

    vector<canPacket> vCanID222;

    fCanDump.open("dump_can.txt");

    putCmdFromCanDump(fCanDump, vCanID222);


    return 0;
}

