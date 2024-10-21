#pragma once

#include <cstdint>
#include <cstdlib>
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

// File IO functions
extern std::map<std::string, int> formatMap;

// Matrix functions
std::string printMatrix(const std::array<std::array<double, 3>, 3> &matrix);
double determinant3x3(const std::array<std::array<double, 3>, 3> &matrix);

// Debug functions
void Debug(const std::string &message, const std::int16_t &level);

int randomID();
} // namespace dsutil
