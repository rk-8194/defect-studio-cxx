#include "dsutil.h"
#include "global.h"
#include <array>
#include <cstdint>
#include <format>
#include <iostream>
#include <string>

#include <string>

void dsutil::Debug(const std::string &message, const std::int16_t &level)
{
    if (g_debugMode && g_verbosity >= level)
        std::cout << format("[{}]\t{}", level, message) << std::endl;
}

std::string dsutil::getMatrix(const std::array<std::array<double, 3>, 3> &matrix)
{
    return std::format("{} {} {}\n\t{} {} {}\n\t{} {} {}", matrix[0][0], matrix[0][1], matrix[0][2], matrix[1][0],
                       matrix[1][1], matrix[1][2], matrix[2][0], matrix[2][1], matrix[2][2]);
}

std::map<std::string, int> dsutil::formatMap = {{"VASP", 1}, {"XYZ", 2}};
