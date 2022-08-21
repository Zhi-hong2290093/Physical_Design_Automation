#include "Parser.hpp"
#include <fstream>
#include <iostream>

void Parser::readCell(std::string const &filename)
{
    std::ifstream fin(filename);
    std::string buffer;
    std::string name;
    int size;

    if (!fin.is_open())
    {
        std::cerr << "Open Cell File Error";
        exit(1);
    }
    while (fin >> buffer)
    {
        name = buffer;
        fin >> buffer;
        size = std::stoi(buffer);
        auto cell = std::make_shared<Cell>(name, size);
        cell_list.emplace_back(cell);
        cell_dict.insert(std::pair<std::string, Cell::ptr_c>(name, cell));
    }
}

void Parser::readNet(std::string const &filename)
{
    std::ifstream fin(filename);
    std::string buffer;
    std::string name;

    if (!fin.is_open())
    {
        std::cerr << "Open Net File Error";
        exit(1);
    }
    while (fin >> buffer)
    {
        fin >> name >> buffer >> buffer;
        auto net = std::make_shared<Net>(name);
        while (buffer[0] != '}')
        {
            cell_dict[buffer]->nets.emplace_back(net);
            net->cells.emplace_back(cell_dict[buffer]);
            fin >> buffer;
        }
        net_list.emplace_back(net);
    }
}

FMInput *Parser::parse(char *argv[])
{
    readCell(argv[1]);
    readNet(argv[2]);

    return new FMInput(cell_list, net_list, cell_dict);
}