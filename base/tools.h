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
    static void set_log_fuction(std::function<void(const std::string&)> log_func);
    static void message(const std::string& text);

private:
    static std::function<void(const std::string&)> m_log_func;
};

//-----------------------------------------------------------------------------

int random(int rangeMin, int rangeMax);

    } // namespace base
} // namespace sim

#endif // TOOLS_H
