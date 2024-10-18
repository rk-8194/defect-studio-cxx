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

/// <summary>
/// Get an argument by index, returning it as a pair.
/// </summary>
/// <param name="index">The index of the argument in the list.</param>
std::pair<const std::string, std::vector<std::string>> CommandArguments::getArgument(const int &index)
{
    auto it = list.begin();
    std::advance(it, index);

    return *it;
}

/// <summary>
/// Finds the argument in the list and returns its values.
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
std::vector<std::string> CommandArguments::findArgument(const std::string &key)
{
    auto it = list.find(key);

    if (it != list.end())
    {
        return it->second;
    }
    else
    {
        dsutil::Debug(std::format("The argument {} was not found.", key), -1);
        return {};
    }
}

bool CommandArguments::hasArgument(const std::string &key)
{
    auto it = list.find(key);

    if (it != list.end())
    {
        dsutil::Debug(std::format("The argument {} was found.", key), 2);
        return true;
    }
    else
    {
        dsutil::Debug(std::format("The argument {} was not found.", key), 2);
        return false;
    }
}
