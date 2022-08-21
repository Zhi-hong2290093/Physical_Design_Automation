#include "Parser.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <random>

void Parser::readBlock(std::string const &filename, const float &WhiteSpaceRatio, const int &seed)
{
    std::ifstream fin(filename);
    std::string buffer;
    std::string name;
    int num;
    int x0, x1, y0, y1;

    if (!fin.is_open())
    {
        std::cerr << "Open Block File Error";
        exit(1);
    }

    fin >> buffer >> buffer >> num_block;
    fin >> buffer >> buffer >> num_terminal;

    for (int i = 0; i < num_block; i++)
    {
        fin >> name >> buffer >> num;
        fin >> buffer;
        buffer = buffer.substr(1, buffer.size() - 2);
        x0 = std::atoi(buffer.c_str());

        fin >> buffer;
        buffer = buffer.substr(0, buffer.size() - 1);
        y0 = std::atoi(buffer.c_str());

        fin >> buffer >> buffer;
        buffer = buffer.substr(0, buffer.size() - 1);
        y1 = std::atoi(buffer.c_str());

        fin >> buffer >> buffer >> buffer;
        buffer = buffer.substr(1, buffer.size() - 2);
        x1 = std::atoi(buffer.c_str());

        fin >> buffer;
        area += (x1 - x0) * (y1 - y0);
        auto node = std::make_shared<Node>(name, i, x0, y0, y1 - y0, x1 - x0);
        block_list.emplace_back(node);
    }

    srand(seed);
    std::random_shuffle(std::begin(block_list), std::end(block_list), [&](int i)
                        { return std::rand() % i; });

    for (size_t i = 0; i < block_list.size(); ++i)
        block_list.at(i)->id = i;

    outline = std::sqrt(area * (1.0 + WhiteSpaceRatio));
    fin.close();
}

void Parser::readNet(std::string const &filename)
{
    std::ifstream fin(filename);
    std::string buffer;
    std::string name;
    int num_net, degree;

    if (!fin.is_open())
    {
        std::cerr << "Open Net File Error";
        exit(1);
    }

    fin >> buffer >> buffer >> num_net;
    fin >> buffer >> buffer >> buffer;

    for (int i = 0; i < num_net; i++)
    {
        fin >> buffer >> buffer >> degree;
        auto net = std::make_shared<Net>(degree);

        for (int j = 0; j < degree; j++)
        {
            fin >> name;
            if (name[0] == 'p')
                net->pins.emplace_back(name);
            else
            {
                for (auto i : block_list)
                    if (i->name == name)
                        net->blocks.emplace_back(i->id);
            }
        }
        net_list.emplace_back(net);
    }
    fin.close();
}

void Parser::readPL(std::string const &filename)
{
    std::ifstream fin(filename);
    std::string name;
    int x, y;

    if (!fin.is_open())
    {
        std::cerr << "Open PL File Error";
        exit(1);
    }

    for (int i = 0; i < num_terminal; i++)
    {
        fin >> name >> x >> y;
        auto pin = std::make_shared<Pin>(name, x, y);
        pin_dic.insert(std::pair<std::string, Pin::ptr>(name, pin));
        pin_list.emplace_back(pin);
    }
    fin.close();
}

SAInput *Parser::parse(char *argv[])
{
    readBlock(argv[1], std::atof(argv[5]), std::atoi(argv[6]));
    readNet(argv[2]);
    readPL(argv[3]);
    std::cout << "seed: " << std::atoi(argv[6]) << ",";
    return new SAInput(block_list, pin_list, net_list, pin_dic, outline, area);
}