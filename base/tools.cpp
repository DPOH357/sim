#include "tools.h"

#include <iostream>
#include <stdlib.h>

int sim::base::random(int rangeMin, int rangeMax)
{
    if(rangeMin > rangeMax)
    {
        int tmp = rangeMin;
        rangeMin = rangeMax;
        rangeMax = tmp;
    }
    return (rand() / (RAND_MAX + 1) \
        * (rangeMax - rangeMin) + rangeMin);
}
