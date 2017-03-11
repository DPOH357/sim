#ifndef LOG_H
#define LOG_H

#include <export.h>
#include <functional>

namespace sim
{


class SIMLIB_API log
{
    log() {}

public:
    enum class level
    {
        Error   = 1,// only error messages
        Warning = 2,// warning and errors
        Debug   = 3 // all messages
    };

    static void set_log_level(log::level log_level);
    static void set_log_fuction(std::function<void(log::level level, const std::string&)> log_func);
    static void message(log::level log_level, const std::string& text);

private:
    static void log_function_default(level level, const std::string& text);

private:
    static log::level                               ms_log_level;
    static std::function<void(log::level level, const std::string&)>  ms_log_func;
};


} // namespace sim

#endif // LOG_H
