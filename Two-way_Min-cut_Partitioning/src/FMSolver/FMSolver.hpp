#pragma once
#include "../Data/Data.hpp"
#include <unordered_map>
#include <vector>
#include "../GlobalTimer/GlobalTimer.hpp"

class FMSolver
{
    FMInput *input;
    GlobalTimer &globalTimer;
    std::unordered_map<int, Entry::ptr> entry_map1;
    std::unordered_map<int, Entry::ptr> entry_map2;
    int max_gain, min_gain, cell_number, area_a, area_b;

public:
    FMSolver(FMInput *input,GlobalTimer &globalTimer) : input(input),globalTimer(globalTimer) , cell_number(0), area_a(0), area_b(0) {}
    
    void initial_partition(int seed);
    void build_bucket_list();
    int compute_cutsize();
    void update_bucket_list(Cell::ptr_c cell, int source, int target);
    int move_cell(Cell::ptr_c cell);
    void initial();
    bool fm_process();
    void solve(int seed);
    void write_result(std::string name);
};