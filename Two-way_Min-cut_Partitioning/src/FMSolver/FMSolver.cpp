#include "FMSolver.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <fstream>

void FMSolver::initial_partition(int seed)
{
    srand(seed);
    std::random_shuffle(std::begin(input->cell_list), std::end(input->cell_list), [&](int i)
                        { return std::rand() % i; });
    for (auto i : input->cell_list)
    {
        if (area_a < area_b)
        {
            i->set = 0;
            area_a += i->size;
        }
        else
        {
            i->set = 1;
            area_b += i->size;
        }
    }
    for (auto i : input->net_list)
    {
        for (auto j : i->cells)
        {
            if (!j->set)
                i->a_count++;
            else
                i->b_count++;
        }
    }
    cell_number = input->cell_list.size();
}

void FMSolver::build_bucket_list()
{
    max_gain = -100, min_gain = 100;
    for (auto i : input->cell_list)
    {
        for (auto j : i->nets)
        {
            if (j->a_count == 0 || j->b_count == 0)
                i->gain--;
            else if (j->a_count == 1 && i->set == 0)
                i->gain++;
            else if (j->b_count == 1 && i->set == 1)
                i->gain++;
        }
        if (i->gain > max_gain)
            max_gain = i->gain;
        if (i->gain < min_gain)
            min_gain = i->gain;
    }
    for (int i = max_gain; i >= min_gain; i--)
    {
        auto entry1 = std::make_shared<Entry>(i);
        auto entry2 = std::make_shared<Entry>(i);
        entry_map1.insert(std::pair<int, Entry::ptr>(i, entry1));
        entry_map2.insert(std::pair<int, Entry::ptr>(i, entry2));
    }

    for (auto i : input->cell_list)
    {
        std::shared_ptr<Entry> tmp;
        if (!i->set)
            tmp = entry_map1[i->gain];
        else
            tmp = entry_map2[i->gain];

        if (!tmp->next)
            tmp->next = tmp->last = i;
        else
        {
            tmp->last->next = i;
            i->prev = tmp->last;
            tmp->last = i;
        }
    }
}

int FMSolver::compute_cutsize()
{
    int count = 0;
    for (auto i : input->net_list)
    {
        int a = 0, b = 0;
        for (auto j : i->cells)
        {
            if (j->set)
                a++;
            else
                b++;
        }
        if (a != 0 && b != 0)
            count++;
    }
    return count;
}

void FMSolver::update_bucket_list(Cell::ptr_c cell, int source, int target)
{
    if (!cell->prev)
    {
        if (cell->set == 0)
            entry_map1[source]->next = cell->next;
        else
            entry_map2[source]->next = cell->next;
        if (cell->next)
            cell->next->prev = nullptr;
    }
    else
    {
        auto prev = cell->prev;
        auto next = cell->next;
        prev->next = next;
        if (next)
            next->prev = prev;
    }
    if (target <= max_gain && target >= min_gain)
    {
        std::shared_ptr<Entry> entry;
        if (cell->set == 0)
            entry = entry_map1[target];
        else
            entry = entry_map2[target];

        if (!entry->next)
        {
            entry->next = cell;
            cell->next = nullptr;
            cell->prev = nullptr;
        }
        else
        {
            auto tmp = entry->next;
            while (tmp->next)
                tmp = tmp->next;
            tmp->next = cell;
            cell->prev = tmp;
            cell->next = nullptr;
        }
    }
    else
    {
        if (target > max_gain)
        {
            for (int i = max_gain; i <= target; i++)
            {
                auto entry1 = std::make_shared<Entry>(i);
                auto entry2 = std::make_shared<Entry>(i);
                entry_map1.insert(std::pair<int, Entry::ptr>(i, entry1));
                entry_map2.insert(std::pair<int, Entry::ptr>(i, entry2));
            }
            max_gain = target;
        }
        else if (target < min_gain)
        {
            for (int i = min_gain; i >= target; --i)
            {
                auto entry1 = std::make_shared<Entry>(i);
                auto entry2 = std::make_shared<Entry>(i);
                entry_map1.insert(std::pair<int, Entry::ptr>(i, entry1));
                entry_map2.insert(std::pair<int, Entry::ptr>(i, entry2));
            }
            min_gain = target;
        }
        std::shared_ptr<Entry> entry;
        if (cell->set == 0)
            entry = entry_map1[target];
        else
            entry = entry_map2[target];
        entry->next = cell;
        cell->next = nullptr;
        cell->prev = nullptr;
    }
}

int FMSolver::move_cell(Cell::ptr_c cell)
{
    int gain = cell->gain;
    if (cell->set == 0)
        entry_map1[gain]->next = cell->next;
    else
        entry_map2[gain]->next = cell->next;
    if (cell->next)
        cell->next->prev = nullptr;

    cell->lock = true;

    for (auto i : cell->nets)
    {
        if (cell->set == 0)
        {
            if (i->b_count == 0)
            {
                for (auto j : i->cells)
                {
                    int source = j->gain;
                    if (!j->lock)
                    {
                        j->gain++;
                        update_bucket_list(j, source, j->gain);
                    }
                }
            }
            else if (i->b_count == 1)
            {
                for (auto j : i->cells)
                {
                    if (j->set == 1 && !j->lock)
                    {
                        int source = j->gain;
                        j->gain--;
                        update_bucket_list(j, source, j->gain);
                    }
                }
            }
            i->a_count--;
            i->b_count++;
        }
        else
        {
            if (i->a_count == 0)
            {
                for (auto j : i->cells)
                {
                    int source = j->gain;
                    if (!j->lock)
                    {
                        j->gain++;
                        update_bucket_list(j, source, j->gain);
                    }
                }
            }
            else if (i->a_count == 1)
            {
                for (auto j : i->cells)
                {
                    if (j->set == 0 && !j->lock)
                    {
                        int source = j->gain;
                        j->gain--;
                        update_bucket_list(j, source, j->gain);
                    }
                }
            }
            i->a_count++;
            i->b_count--;
        }
        int tmp = (cell->set + 1) % 2;
        if (i->a_count == 0 || i->b_count == 0)
        {
            for (auto j : i->cells)
            {
                if (!j->lock)
                {
                    int source = j->gain;
                    j->gain--;
                    update_bucket_list(j, source, j->gain);
                }
            }
        }
        else if (tmp == 1 && i->a_count == 1)
        {
            for (auto j : i->cells)
            {
                if (!j->lock && j->set == 0)
                {
                    int source = j->gain;
                    j->gain++;
                    update_bucket_list(j, source, j->gain);
                }
            }
        }
        else if (tmp == 0 && i->b_count == 1)
        {
            for (auto j : i->cells)
            {
                if (!j->lock && j->set == 1)
                {
                    int source = j->gain;
                    j->gain++;
                    update_bucket_list(j, source, j->gain);
                }
            }
        }
    }
    cell->set = (cell->set + 1) % 2;
    if (cell->set)
    {
        area_b += cell->size;
        area_a -= cell->size;
    }
    else
    {
        area_a += cell->size;
        area_b -= cell->size;
    }

    return gain;
}

void FMSolver::initial()
{
    for (auto i : input->cell_list)
    {
        i->lock = false;
        i->gain = 0;
        i->next = nullptr;
        i->prev = nullptr;
    }
    entry_map1.clear();
    entry_map2.clear();
}

bool FMSolver::fm_process()
{
    std::vector<std::string> cell_buffer;
    size_t count = 0, negative_count = 0;
    int gain = 0, best_gain = 0, best_step = 0, cell_gain = 0;
    while (count < input->cell_list.size() && negative_count < input->cell_list.size() / 25)
    {
        int max_a = -100, max_b = -100;
        std::shared_ptr<Cell> cell;

        for (int i = max_gain; i >= min_gain; --i)
        {
            if (entry_map1[i]->next && i > max_a)
                max_a = i;
            if (entry_map2[i]->next && i > max_b)
                max_b = i;
        }

        if (max_a >= max_b)
        {
            cell = entry_map1[max_a]->next;
            if (abs(area_a - area_b - 2 * cell->size) >= input->cell_list.size() / 10)
                cell = entry_map2[max_b]->next;
        }
        else
        {
            cell = entry_map2[max_b]->next;
            if (abs(area_b - area_a - 2 * cell->size) >= input->cell_list.size() / 10)
                cell = entry_map1[max_a]->next;
        }

        cell_gain = move_cell(cell);
        gain += cell_gain;

        if (gain > best_gain)
        {
            best_gain = gain;
            best_step = count;
        }
        if (cell_gain < 0)
            negative_count++;

        count++;
        cell_buffer.emplace_back(cell->name);
    }

    for (size_t i = best_step + 1; i < cell_buffer.size(); i++)
    {
        if (input->cell_dict[cell_buffer[i]]->set)
        {
            area_a += input->cell_dict[cell_buffer[i]]->size;
            area_b -= input->cell_dict[cell_buffer[i]]->size;
            for (auto j : input->cell_dict[cell_buffer[i]]->nets)
            {
                j->a_count++;
                j->b_count--;
            }
        }
        else
        {
            area_a -= input->cell_dict[cell_buffer[i]]->size;
            area_b += input->cell_dict[cell_buffer[i]]->size;
            for (auto j : input->cell_dict[cell_buffer[i]]->nets)
            {
                j->b_count++;
                j->a_count--;
            }
        }
        input->cell_dict[cell_buffer[i]]->set = (input->cell_dict[cell_buffer[i]]->set + 1) % 2;
    }

    if (best_gain > 0)
        return true;
    else
        return false;
}

void FMSolver::solve(int seed)
{
    initial_partition(seed);
    int max_iter = 5, iter = 0, count = 0;
    while (iter < max_iter && !globalTimer.overTime())
    {
        initial();
        build_bucket_list();
        if (!fm_process())
            iter++;
        count++;
    }
}

void FMSolver::write_result(std::string name)
{
    std::ofstream fout;
    fout.open(name);
    int b_count = 0;
    fout << "cut_size " << compute_cutsize() << std::endl;
    for (auto i : input->cell_list)
        if (i->set)
            b_count++;
    fout << "A " << input->cell_list.size() - b_count << std::endl;
    for (auto i : input->cell_list)
        if (!i->set)
            fout << i->name << std::endl;
    fout << "B " << b_count << std::endl;
    for (auto i : input->cell_list)
        if (i->set)
            fout << i->name << std::endl;
}