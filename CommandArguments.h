#pragma once

#include "dsutil.h"
#include <format>
#include <map>
#include <string>
#include <vector>

class CommandArguments
{
  public:
    CommandArguments();
    CommandArguments(const std::string &key, const std::vector<std::string> &values);
    std::map<std::string, std::vector<std::string>> list;

    void printArguments();
    void addArgument(const std::string &key, const std::vector<std::string> &values);
    std::pair<const std::string, std::vector<std::string>> getArgument(const int &index);
    std::vector<std::string> findArgument(const std::string &key);
    bool hasArgument(const std::string &key);
};
