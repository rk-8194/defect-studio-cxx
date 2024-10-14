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
    if (level > 0)
    {
        if (!g_debugMode)
            return;
        else if (g_verbosity >= level)
            std::cout << format("[{}]\t{}", level, message) << std::endl;
    }
    else if (level == 0)
        std::cout << format("\t{}", message) << std::endl;
    else
        std::cout << format("[!!!]\t{}", message) << std::endl;
}

std::string dsutil::printMatrix(const std::array<std::array<double, 3>, 3> &matrix)
{
    return std::format("{} {} {}\n\t{} {} {}\n\t{} {} {}", matrix[0][0], matrix[0][1], matrix[0][2], matrix[1][0],
                       matrix[1][1], matrix[1][2], matrix[2][0], matrix[2][1], matrix[2][2]);
}

std::map<std::string, int> dsutil::formatMap = {{"VASP", 1}, {"XYZ", 2}};
