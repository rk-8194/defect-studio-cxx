#pragma once

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <tuple>

namespace dsutil
{

/// @brief Struct for 3 dimensional vector.
struct Vector3D
{
    double x, y, z;

    Vector3D() : x(0), y(0), z(0)
    {
    }
    Vector3D(double xVal, double yVal, double zVal) : x(xVal), y(yVal), z(zVal)
    {
    }

    // Magnitude (length) of the vector
    double magnitude() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Normalize the vector in-place
    void normalize()
    {
        double mag = magnitude();
        if (mag > 1e-8)
        {
            x /= mag;
            y /= mag;
            z /= mag;
        }
    }

    // Return a normalized copy
    Vector3D normalized() const
    {
        double mag = magnitude();
        if (mag > 1e-8)
        {
            return Vector3D(x / mag, y / mag, z / mag);
        }
        return Vector3D(0, 0, 0);
    }

    // Dot product
    double dot(const Vector3D &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vector3D cross(const Vector3D &other) const
    {
        return Vector3D(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    // Operator overloads for vector arithmetic
    Vector3D operator+(const Vector3D &other) const
    {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }

    Vector3D operator-(const Vector3D &other) const
    {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }

    Vector3D operator*(double scalar) const
    {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector3D operator/(double scalar) const
    {
        return Vector3D(x / scalar, y / scalar, z / scalar);
    }

    // Comparator for std::set
    bool operator<(const Vector3D &other) const
    {
        return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
    }

    // Operator to scale a vector by a scalar (modifies the original vector)
    Vector3D &operator*=(double scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
};

struct Matrix3x3
{
    double m[3][3];

    // Matrix-vector multiplication
    Vector3D operator*(const Vector3D &v) const
    {
        return Vector3D{m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z, m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
                        m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z};
    }

    // Print matrix to console
    void print() const
    {
        std::cout << std::fixed << std::setprecision(4);
        for (int i = 0; i < 3; ++i)
        {
            std::cout << "| ";
            for (int j = 0; j < 3; ++j)
            {
                std::cout << m[i][j] << " ";
            }
            std::cout << "|\n";
        }
    }

    // Compute determinant
    double determinant() const
    {
        return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
               m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }

    // Return the transpose of the matrix
    Matrix3x3 transpose() const
    {
        Matrix3x3 result{};
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                result.m[i][j] = m[j][i];
        return result;
    }

    // Return the inverse of the matrix (throws if not invertible)
    Matrix3x3 inverse() const
    {
        double det = determinant();
        Matrix3x3 inv;

        inv.m[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) / det;
        inv.m[0][1] = -(m[0][1] * m[2][2] - m[0][2] * m[2][1]) / det;
        inv.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) / det;

        inv.m[1][0] = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]) / det;
        inv.m[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) / det;
        inv.m[1][2] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]) / det;

        inv.m[2][0] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]) / det;
        inv.m[2][1] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]) / det;
        inv.m[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) / det;

        return inv;
    }

    // Return identity matrix
    static Matrix3x3 identity()
    {
        return Matrix3x3{{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}};
    }
};

// Matrix functions
std::string printMatrix(const std::array<std::array<double, 3>, 3> &matrix);
double determinant3x3(const std::array<std::array<double, 3>, 3> &matrix);

// File IO functions
extern std::map<std::string, int> formatMap;

// Debug functions
void Debug(const std::string &message, const std::int16_t &level);

int randomID();
} // namespace dsutil
