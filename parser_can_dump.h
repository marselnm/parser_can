#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

class canPacket
{
public:
    canPacket(string strLine)
    {
        str_packet_ = strLine;
        convStrInBytes(); 
    }

    void convStrInBytes()
    {
        size_t pos = str_packet_.find("Rx");

        int temp_size = str_packet_.size();

        if (pos != std::string::npos && str_packet_.size() >= 53)
        {
            for (int i = 0; i < 8; ++i)
            {
                uint8_t msb = convSymbolToDec(str_packet_.at(pos + 5 + 3*i));
                uint8_t lsb = convSymbolToDec(str_packet_.at(pos + 5 + 3*i + 1)); 

                bytes_[i] = (msb << 4) | lsb;
            }
        } 
    }

    uint8_t convSymbolToDec(uint8_t symb)
    {
        uint8_t temp_symb = 0;

        if (symb >= 'A' && symb <= 'F')
        {
            temp_symb = symb - 55;
        }
        else if (symb >= '0' && symb <= '9')
        {
            temp_symb = symb - 48;
        }

        return (temp_symb & 0xF);
    }

    void printBytes()
    {
        for (int i = 0; i < 8; ++i)
        {
            printf("%X ", bytes_[i]); 
        }
        printf("\n");
    }

    string str_packet_;
    uint8_t bytes_[8];
};

void putCmdFromCanDump(std::ifstream& fCanDump, vector<canPacket>& vCan, string part_of_str="0x222 STD Rx 8 ");