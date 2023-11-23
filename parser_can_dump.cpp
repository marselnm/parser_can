#include "parser_can_dump.h"

void putCmdFromCanDump(std::ifstream& fCanDump, vector<canPacket>& vCan, string part_of_str)
{
    if (fCanDump.is_open())
    {
        std::string temp_str;

        while(!fCanDump.eof())
        {
            temp_str = "";
            std::getline(fCanDump, temp_str);

            if (temp_str.find(part_of_str) != std::string::npos)
            {
                canPacket packet(temp_str);
                vCan.push_back(packet);

                std::cout << temp_str << std::endl;//for debug only
                packet.printBytes();//for debug only
            }
        }
    }
}