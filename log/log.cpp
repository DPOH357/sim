#include "log.h"

#include <iostream>

sim::log::level sim::log::ms_log_level = sim::log::level::Error;
std::function<void(sim::log::level level, const std::string&)> sim::log::ms_log_func = std::bind(sim::log::log_function_default, std::placeholders::_1, std::placeholders::_2);

void sim::log::set_log_level(sim::log::level log_level)
{
    ms_log_level = log_level;
}

void sim::log::set_log_fuction(std::function<void (log::level level, const std::string &)> log_func)
{
    ms_log_func = log_func;
}

void sim::log::message(log::level log_level, const std::string &text)
{
    if((unsigned int)log_level & (unsigned int)ms_log_level)
    {
        ms_log_func(log_level, text);
    }
}

void sim::log::log_function_default(log::level level, const std::string &text)
{
    std::string log_level_name;
    switch(level)
    {
    case log::level::Error:     log_level_name = "Error: "; break;
    case log::level::Warning:   log_level_name = "Warning"; break;
    case log::level::Debug:     log_level_name = "Debug"; break;
    default: break;
    }

    std::cout << log_level_name << text << std::endl;
}
