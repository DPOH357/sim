#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <functional>

namespace sim
{
    namespace base
    {


void log_function_default(const std::string& text);

class log
{
public:

    enum class level
    {
        Error   = 1,// only error messages
        Warning = 2,// warning and errors
        Info    = 3 // all messages
    };

    static void set_log_level(log::level log_level);
    static void set_log_fuction(std::function<void(const std::string&)> log_func);
    static void message(log::level log_level, const std::string& text);

private:
    static log::level                               ms_log_level;
    static std::function<void(const std::string&)>  ms_log_func;
};

//-----------------------------------------------------------------------------

int random(int rangeMin, int rangeMax);

    } // namespace base
} // namespace sim

#endif // TOOLS_H
