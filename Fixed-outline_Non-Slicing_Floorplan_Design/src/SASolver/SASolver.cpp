#include "SASolver.hpp"
#include "../Data/Data.hpp"
#include <cmath>
#include <fstream>

int SASolver::initialize_b_star_tree(const double &layout)
{
    int root = 0;
    int current_x = input->block_list.at(root)->width, current_left_node = root;

    for (size_t i = 1; i < input->block_list.size(); ++i)
    {
        if (current_x + input->block_list.at(i)->width <= layout)
        {
            input->block_list.at(i - 1)->leftchild = i;
            input->block_list.at(i)->parent = i - 1;
            current_x += input->block_list.at(i)->width;
        }
        else
        {
            input->block_list.at(current_left_node)->rightchild = i;
            input->block_list.at(i)->parent = current_left_node;
            input->block_list.at(i)->is_left_child = false;
            current_x = input->block_list.at(i)->width;
            current_left_node = i;
        }
    }
    return root;
}

void SASolver::constrct_new_list()
{
    for (auto i : input->block_list)
    {
        auto node = std::make_shared<Node>(i->name, i->id, i->x, i->y, i->height, i->width);
        new_block_list.emplace_back(node);
    }
    copy_list(input->block_list, new_block_list);
}

bool SASolver::initialize_contour()
{
    while (contour)
    {
        auto ptr(contour);
        contour = contour->next;
        if (contour)
            contour->prev = nullptr;
        ptr.reset();
    }
    return true;
}

int SASolver::update_contour(const Node::ptr node)
{
    int max_y = 0;
    if (!contour)
    {
        contour = std::make_shared<Contour>(0, node->width, node->height);
        return 0;
    }
    else
    {
        auto current(contour);
        Contour::ptr new_contour = std::make_shared<Contour>(node->x, node->x + node->width, 0);
        Contour::ptr first, second;
        bool overlape = false;
        while (current)
        {
            if (current->overlape(node->x, node->x + node->width) == 0) //intersect a contour
            {
                overlape = true;
                if (max_y < current->y)
                    max_y = current->y;
                if (current == contour)
                {
                    new_contour->next = contour;
                    new_contour->next->prev = new_contour;
                    contour->x1 = node->width;
                    contour = new_contour;
                }
                else
                {
                    current->x1 = node->x + node->width;
                    if (current->prev != new_contour)
                    {
                        new_contour->prev = current->prev;
                        current->prev->next = new_contour;
                    }
                    new_contour->next = current;
                    current->prev = new_contour;
                }
            }
            else if (current->overlape(node->x, node->x + node->width) == 1) //cover a contour
            {
                overlape = true;
                if (max_y < current->y)
                    max_y = current->y;
                if (current == contour)
                {
                    new_contour->next = contour->next;
                    if (new_contour->next)
                        new_contour->next->prev = new_contour;
                    contour = new_contour;
                }
                else
                {
                    if (new_contour->prev != new_contour && current->prev != new_contour)
                    {
                        new_contour->prev = current->prev;
                        current->prev->next = new_contour;
                    }
                    new_contour->next = current->next;
                    if (current->next)
                        current->next->prev = new_contour;
                }
            }

            if (!current->next)
                break;
            current = current->next;
        }
        if (!overlape)
        {
            current->next = new_contour;
            new_contour->prev = current;
            new_contour->y = node->height;
        }
        else
            new_contour->y = max_y + node->height;

        return max_y;
    }
}

void SASolver::update_coordinate(const int &id, const int &current_x, std::vector<Node::ptr> &list)
{
    list.at(id)->x = current_x;
    list.at(id)->y = update_contour(list.at(id));

    if (list.at(id)->leftchild >= 0)
        update_coordinate(list.at(id)->leftchild, current_x + list.at(id)->width, list);
    if (list.at(id)->rightchild >= 0)
        update_coordinate(list.at(id)->rightchild, current_x, list);
}

float SASolver::compute_cost(const int layout, const bool &focusWirelength)
{
    double wirelength = 0, layout_x = 0, layout_y = 0, max_x, max_y, min_x, min_y, area;

    for (auto i : input->net_list)
    {
        max_x = 0, max_y = 0, min_x = 10000, min_y = 10000;
        for (auto j : i->blocks)
        {
            double x = new_block_list.at(j)->x + new_block_list.at(j)->width / 2;
            double y = new_block_list.at(j)->y + new_block_list.at(j)->height / 2;
            max_x = (max_x < x) ? x : max_x;
            max_y = (max_y < y) ? y : max_y;
            min_x = (x < min_x) ? x : min_x;
            min_y = (y < min_y) ? y : min_y;
            if (layout_x < new_block_list.at(j)->x + new_block_list.at(j)->width)
                layout_x = new_block_list.at(j)->x + new_block_list.at(j)->width;
            if (layout_y < new_block_list.at(j)->y + new_block_list.at(j)->height)
                layout_y = new_block_list.at(j)->y + new_block_list.at(j)->height;
        }

        for (auto j : i->pins)
        {
            max_x = (max_x < input->pin_dic.at(j)->x) ? input->pin_dic.at(j)->x : max_x;
            max_y = (max_y < input->pin_dic.at(j)->y) ? input->pin_dic.at(j)->y : max_y;
            min_x = (input->pin_dic.at(j)->x < min_x) ? input->pin_dic.at(j)->x : min_x;
            min_y = (input->pin_dic.at(j)->y < min_y) ? input->pin_dic.at(j)->y : min_y;
        }
        wirelength += (max_x - min_x + max_y - min_y);
    }

    if (layout_x > layout && layout_y > layout)
        area = (layout_x * layout_y) - (layout * layout);
    else if (layout_x > layout && layout_y <= layout)
        area = (layout_x - layout) * layout;
    else if (layout_x <= layout && layout_y > layout)
        area = (layout_y - layout) * layout;
    else
        area = 0;

    if (!focusWirelength)
        return area * 10;
    else
        return area * 1000 + wirelength;
}

void SASolver::copy_list(std::vector<Node::ptr> &list1, std::vector<Node::ptr> &list2)
{
    for (auto i : list1)
        *list2.at(i->id) = *list1.at(i->id);
}

void SASolver::update_node(const int node, const int new_parent, const int new_lchild, const int new_rchild, const bool is_left, std::vector<Node::ptr> &list)
{
    list.at(node)->parent = new_parent;
    list.at(node)->rightchild = new_rchild;
    list.at(node)->leftchild = new_lchild;
    if (new_parent >= 0)
    {
        if (is_left)
            list.at(new_parent)->leftchild = node;
        else
            list.at(new_parent)->rightchild = node;
        list.at(node)->is_left_child = is_left;
    }
    if (new_rchild >= 0)
    {
        list.at(new_rchild)->parent = node;
        list.at(new_rchild)->is_left_child = false;
    }
    if (new_lchild >= 0)
    {
        list.at(new_lchild)->parent = node;
        list.at(new_lchild)->is_left_child = true;
    }
}

void SASolver::perturb(int &root, const int &type, std::vector<Node::ptr> &list)
{
    int node1 = rand() % list.size();
    int node2 = rand() % list.size();
    if (node1 == node2)
        node2 = (node2 + 1) % list.size();
    int node1_parent = list.at(node1)->parent, node1_lchild = list.at(node1)->leftchild, node1_rchild = list.at(node1)->rightchild;
    bool node1_is_left_child = list.at(node1)->is_left_child;

    if (type == 0)
    {
        int height = list.at(node1)->height;
        list.at(node1)->height = list.at(node1)->width;
        list.at(node1)->width = height;
        list.at(node1)->rotate = (list.at(node1)->rotate + 1) % 2;
    }
    else if (type == 1)
    {
        int node2_parent = list.at(node2)->parent, node2_lchild = list.at(node2)->leftchild, node2_rchild = list.at(node2)->rightchild;
        bool node2_is_left_child = list.at(node2)->is_left_child;

        if (node1_parent == node2)
        {
            if (list.at(node1)->is_left_child)
                update_node(node1, node2_parent, node2, node2_rchild, node2_is_left_child, list);
            else
                update_node(node1, node2_parent, node2_lchild, node2, node2_is_left_child, list);
            update_node(node2, node1, node1_lchild, node1_rchild, node1_is_left_child, list);
        }
        else if (node2_parent == node1)
        {
            if (list.at(node2)->is_left_child)
                update_node(node2, node1_parent, node1, node1_rchild, node1_is_left_child, list);
            else
                update_node(node2, node1_parent, node1_lchild, node1, node1_is_left_child, list);
            update_node(node1, node2, node2_lchild, node2_rchild, node2_is_left_child, list);
        }
        else
        {
            update_node(node1, node2_parent, node2_lchild, node2_rchild, node2_is_left_child, list);
            update_node(node2, node1_parent, node1_lchild, node1_rchild, node1_is_left_child, list);
        }

        if (node1 == root)
            root = node2;
        else if (node2 == root)
            root = node1;
    }
    else
    {
        int put_left = rand() % 2;

        if (node1_lchild >= 0 && node1_rchild >= 0)
        {
            int new_node1 = node1_lchild;
            bool left = true;
            while (list.at(new_node1)->leftchild >= 0 && list.at(new_node1)->rightchild >= 0)
                new_node1 = list.at(new_node1)->leftchild;
            if (list.at(new_node1)->leftchild >= 0)
                left = false;
            update_node(node1_lchild, node1_parent, list.at(node1_lchild)->leftchild, list.at(node1_lchild)->rightchild, node1_is_left_child, list);
            if (left)
                update_node(new_node1, list.at(new_node1)->parent, node1_rchild, list.at(new_node1)->rightchild, list.at(new_node1)->is_left_child, list);
            else
                update_node(new_node1, list.at(new_node1)->parent, list.at(new_node1)->leftchild, node1_rchild, list.at(new_node1)->is_left_child, list);
        }
        else
        {
            if (node1_lchild >= 0)
                update_node(node1_lchild, node1_parent, list.at(node1_lchild)->leftchild, list.at(node1_lchild)->rightchild, node1_is_left_child, list);
            else if (node1_rchild >= 0)
                update_node(node1_rchild, node1_parent, list.at(node1_rchild)->leftchild, list.at(node1_rchild)->rightchild, node1_is_left_child, list);
            else if (node1_is_left_child)
                list.at(node1_parent)->leftchild = -1;
            else
                list.at(node1_parent)->rightchild = -1;
        }
        int node2_parent = list.at(node2)->parent, node2_lchild = list.at(node2)->leftchild, node2_rchild = list.at(node2)->rightchild;
        bool node2_is_left_child = list.at(node2)->is_left_child;
        if (put_left)
        {
            update_node(node2, node2_parent, node1, node2_rchild, node2_is_left_child, list);
            update_node(node1, node2, node2_lchild, -1, put_left, list);
        }
        else
        {
            update_node(node2, node2_parent, node2_lchild, node1, node2_is_left_child, list);
            update_node(node1, node2, node2_rchild, -1, put_left, list);
        }
        if (node1 == root)
            root = node1_lchild;
    }
}

void SASolver::simulated_annealing(const double &c, const double &r, const int &k, const bool &focusWirelength, int &root)
{
    int MT = 0, uphill = 0, reject = 0, N = k * input->block_list.size(), layout = input->outline;
    double cost = compute_cost(layout, focusWirelength);
    int new_root = root;
    if (cost == 0)
        return;

    do
    {
        double T0 = 1000;
        do
        {
            MT = uphill = reject = 0;
            do
            {
                if (globalTimer.overTime())
                    return;

                int type = rand() % 2;
                perturb(new_root, type, new_block_list);
                initialize_contour();

                update_coordinate(new_root, 0, new_block_list);

                MT += 1;
                double newCost = compute_cost(layout, focusWirelength);
                double deltaCost = newCost - cost;
                if (deltaCost < 0 || (double)rand() / RAND_MAX < exp(-1 * deltaCost / T0))
                {
                    if (deltaCost > 0)
                        uphill += 1;

                    copy_list(new_block_list, input->block_list);
                    root = new_root;
                    cost = newCost;
                    if (cost == 0)
                        return;
                }
                else
                {
                    copy_list(input->block_list, new_block_list);
                    new_root = root;
                    reject += 1;
                }
            } while (uphill <= N && MT <= 2 * N);
            T0 = r * T0;
        } while (reject / MT <= 0.95 && T0 >= c);
    } while (focusWirelength == false);
}

void SASolver::write_result(const std::string name)
{
    std::ofstream fout;
    fout.open(name);
    fout << "Wirelength " << compute_cost(input->outline, true) << "\n";
    fout << "Blocks\n";
    for (auto s : input->block_list)
        fout << s->name << " " << s->x << " " << s->y << " " << s->rotate << "\n";
}

void SASolver::solve()
{
    auto root = initialize_b_star_tree(input->outline);
    update_coordinate(root, 0, input->block_list);
    constrct_new_list();

    simulated_annealing(0.1, 0.95, 10, false, root);
    simulated_annealing(0.1, 0.9, 10, true, root);
    std::cout << " WL: " << compute_cost(input->outline, true) << "\n";
}