#include "Data.hpp"

int Contour::overlape(const int &x1, const int &x2)
{
    if (this->x1 >= x1 && this->x2 <= x2)
        return 1;
    else if (this->x1 >= x1 && x2 < this->x2 && this->x1 < x2)
        return 0;
    else
        return -1;
}
