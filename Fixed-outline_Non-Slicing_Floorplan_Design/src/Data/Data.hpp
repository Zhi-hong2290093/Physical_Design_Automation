#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct Pin
{
    using ptr = std::shared_ptr<Pin>;
    std::string name;
    int x, y;
    Pin(const std::string &name, const int &x, const int &y) : name(name), x(x), y(y) {}
};

struct Contour
{
    using ptr = std::shared_ptr<Contour>;
    int x1, x2, y;
    Contour::ptr next, prev;

    Contour(const int &x1, const int &x2, const int &y) : x1(x1), x2(x2), y(y), next(nullptr), prev(nullptr) {}
    int overlape(const int &x1, const int &x2);
};

struct Net
{
    using ptr = std::shared_ptr<Net>;
    int degree;
    std::vector<int> blocks;
    std::vector<std::string> pins;

    Net(const int &degree) : degree(degree) {}
};

class Node
{
public:
    using ptr = std::shared_ptr<Node>;
    std::string name;
    int id, parent, leftchild, rightchild, x, y, height, width;
    bool rotate, is_left_child;

    Node(const std::string &name, const int &id, const int &x, const int &y, const int &height, const int &width)
        : name(name), id(id), parent(-1), leftchild(-1), rightchild(-1), x(x), y(y),
          height(height), width(width), rotate(false), is_left_child(true) {}

};

struct SAInput
{
    std::vector<Node::ptr> block_list;
    std::vector<Pin::ptr> pin_list;
    std::vector<Net::ptr> net_list;

    std::unordered_map<std::string, Pin::ptr> pin_dic;
    int outline;
    double area;

    SAInput(std::vector<Node::ptr> &block_list, std::vector<Pin::ptr> &pin_list, std::vector<Net::ptr> &net_list,
            std::unordered_map<std::string, Pin::ptr> pin_dic, const int &outline, const int &area)
        : block_list(block_list), pin_list(pin_list), net_list(net_list), pin_dic(pin_dic), outline(outline), area(area) {}
};