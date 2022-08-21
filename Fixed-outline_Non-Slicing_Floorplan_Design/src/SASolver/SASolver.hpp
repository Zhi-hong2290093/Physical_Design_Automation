#pragma once
#include "../Data/Data.hpp"
#include "../GlobalTimer/GlobalTimer.hpp"

class SASolver
{
    SAInput *input;
    GlobalTimer &globalTimer;
    Contour::ptr contour;

    std::unordered_map<std::string, Pin::ptr> pin_dic;
    std::vector<Node::ptr> new_block_list;

public:
    SASolver(SAInput *input, GlobalTimer &globalTimer) : input(input), globalTimer(globalTimer) {}
    void solve();
    int initialize_b_star_tree(const double &layout);
    void constrct_new_list();
    bool initialize_contour();
    int update_contour(const Node::ptr node);
    void update_coordinate(const int &id, const int &current_x, std::vector<Node::ptr> &list);
    float compute_cost(const int layout, const bool &focusWirelength);
    void copy_list(std::vector<Node::ptr> &list1, std::vector<Node::ptr> &list2);
    void update_node(const int node, const int new_parent, const int new_lchild, const int new_rchild, const bool is_left, std::vector<Node::ptr> &list);
    void perturb(int &root, const int &type, std::vector<Node::ptr> &list);
    void simulated_annealing(const double &c, const double &r, const int &k, const bool &focusWirelength, int &root);
    void write_result(const std::string name);
};