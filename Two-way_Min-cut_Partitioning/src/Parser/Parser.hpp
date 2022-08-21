#pragma once
#include "../Data/Data.hpp"
#include <string>
#include <vector>

class Parser
{
    std::vector<Cell::ptr_c> cell_list;
    std::vector<Net::ptr_n> net_list;
    std::unordered_map<std::string, Cell::ptr_c> cell_dict;
    
    void readCell(std::string const &filename);
    void readNet(std::string const &filename);

public:
    Parser() {}
    FMInput *parse(char *argv[]);
};