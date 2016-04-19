#include "tools.h"

#include <iostream>
#include <stdlib.h>

std::function<void(const std::string&)> sim::base::log::m_log_func = sim::base::log_function_default;

void sim::base::log_function_default(const std::string &text)
{
    std::cout << text << std::endl;
}

void sim::base::log::set_log_fuction(std::function<void (const std::string &)> log_func)
{
    m_log_func = log_func;
}

void sim::base::log::message(const std::string &text)
{
    m_log_func(text);
}

//-----------------------------------------------------------------------------

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




