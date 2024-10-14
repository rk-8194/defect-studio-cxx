#pragma once

#include <cstdint>
#include <iostream>
#include <map>
#include <string>

namespace dsutil
{
struct Vector3D
{
    double x, y, z;

    Vector3D() : x(0), y(0), z(0)
    {
    }

    Vector3D(double xVal, double yVal, double zVal) : x(xVal), y(yVal), z(zVal)
    {
    }
};

extern std::map<std::string, int> formatMap;

std::string printMatrix(const std::array<std::array<double, 3>, 3>& matrix);

void Debug(const std::string &message, const std::int16_t &level);
} // namespace dsutil
