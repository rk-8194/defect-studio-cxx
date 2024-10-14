#pragma once

#include "dsutil.h"
#include <map>
#include <string>
#include <vector>
#include <format>

class CommandArguments
{
  public:
    CommandArguments();
    CommandArguments(const std::string &key, const std::vector<std::string> &values);
    std::map<std::string, std::vector<std::string>> list;

    void printArguments();
    void addArgument(const std::string &key, const std::vector<std::string> &values);
};
