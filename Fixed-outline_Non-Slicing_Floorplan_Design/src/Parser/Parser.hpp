#pragma once
#include "../Data/Data.hpp"
#include <string>
#include <vector>

class Parser
{
    int outline, num_block, num_terminal;
    double area;
    std::vector<Node::ptr> block_list;
    std::vector<Pin::ptr> pin_list;
    std::vector<Net::ptr> net_list;

    std::unordered_map<std::string, Pin::ptr> pin_dic;

    void readBlock(const std::string &filename, const float &WhiteSpaceRatio, const int &seed);
    void readNet(const std::string &filename);
    void readPL(const std::string &filename);

public:
    Parser() : area(0) {}
    SAInput *parse(char *argv[]);
};