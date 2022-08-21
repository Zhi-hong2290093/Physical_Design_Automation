#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct Net;
struct Cell
{
    using ptr_c = std::shared_ptr<Cell>;
    using ptr_n = std::shared_ptr<Net>;
    std::string name;
    int size, set, gain, bucket_index;
    bool lock;
    std::vector<Cell::ptr_n> nets;
    Cell::ptr_c next = nullptr, prev = nullptr;

    Cell(const std::string &name, const int &size) : name(name), size(size), set(-1), gain(0), bucket_index(-1), lock(false) {}
};

struct Net
{
    using ptr_c = std::shared_ptr<Cell>;
    using ptr_n = std::shared_ptr<Net>;
    std::string name;
    std::vector<Net::ptr_c> cells;
    int a_count, b_count;

    Net(const std::string &name) : name(name), a_count(0), b_count(0) {}
};

struct Entry
{
    using ptr = std::shared_ptr<Entry>;
    std::vector<Cell::ptr_c> cells;
    int gain;
    Cell::ptr_c next = nullptr, prev = nullptr, last = nullptr;

    Entry(const int gain) : gain(gain) {}
};

struct FMInput
{
    std::vector<Cell::ptr_c> cell_list;
    std::vector<Net::ptr_n> net_list;
    std::unordered_map<std::string, Cell::ptr_c> cell_dict;

    FMInput(std::vector<Cell::ptr_c> &cell_list, std::vector<Net::ptr_n> &net_list, std::unordered_map<std::string, Cell::ptr_c> &cell_dict)
        : cell_list(cell_list), net_list(net_list), cell_dict(cell_dict){}
};