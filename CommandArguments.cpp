#include "CommandArguments.h"

// Default constructor.
CommandArguments::CommandArguments()
{
}

// Constructor with values.
CommandArguments::CommandArguments(const std::string &key, const std::vector<std::string> &values)
{
    list[key] = values;
}

void CommandArguments::printArguments()
{
    if (list.size() == 0)
        return;

    for (const auto &pair : list)
    {
        dsutil::Debug(std::format("- Argument: {}", pair.first), 2);

        int count = 1;
        for (const auto &value : pair.second)
        {
            dsutil::Debug(std::format("- Value {}: {}", count, value), 2);
            ++count;
        }
    }
}

void CommandArguments::addArgument(const std::string &key, const std::vector<std::string> &values)
{
    list[key] = values;
}
