#include "tools.h"

#include <iostream>
#include <stdlib.h>

sim::base::log::level sim::base::log::ms_log_level = sim::base::log::level::Error;
std::function<void(const std::string&)> sim::base::log::ms_log_func = sim::base::log_function_default;

void sim::base::log_function_default(const std::string &text)
{
    std::cout << text << std::endl;
}

void sim::base::log::set_log_level(sim::base::log::level log_level)
{
    ms_log_level = log_level;
}

void sim::base::log::set_log_fuction(std::function<void (const std::string &)> log_func)
{
    ms_log_func = log_func;
}

void sim::base::log::message(log::level log_level, const std::string &text)
{
    if((unsigned int)log_level & (unsigned int)ms_log_level)
    {
        ms_log_func(text);
    }
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
