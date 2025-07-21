#include "Command.h"
#include "CommandArguments.h"
#include "CrystalStructure.h"
#include "FileWriter.h"
#include "dsutil.h"
#include <cmath>
#include <string>
#include <vector>

using namespace dsutil;

void Surface::execute(CommandArguments &args)
{
    if (!args.hasArgument("AXIS"))
    {
        Debug("Missing required argument: AXIS [h k l]", -1);
        return;
    }

    std::vector<std::string> axis = args.findArgument("AXIS");
    if (axis.size() != 3)
    {
        Debug("AXIS must contain exactly 3 integers.", -1);
        return;
    }

    int h = std::stoi(axis[0]);
    int k = std::stoi(axis[1]);
    int l = std::stoi(axis[2]);

    Debug(std::format("Received Miller indices: <{} {} {}>", h, k, l), 1);

    // Step 1: Define BCC unit cell with lattice parameter a
    double a = 1.0; // lattice constant in Angstroms

    std::array<std::array<double, 3>, 3> unitLattice = {{{a, 0.0, 0.0}, {0.0, a, 0.0}, {0.0, 0.0, a}}};

    Debug("--- BCC Unit Cell Lattice Vectors ---", 1);
    Debug(printMatrix(unitLattice), 1);

    // Step 2: Choose integer n and compute supercell repeat length
    int n = 3;                             // number of repetitions in <hjk> direction
    double na = n * a;                     // length along <hjk>
    double x = std::sqrt(na * na + a * a); // hypotenuse (in-plane repeat)

    Debug(std::format("Assumed number of unit cells n: {}", n), 1);
    Debug(std::format("n.a (length in <hjk>): {:.6f} Angstroms", na), 1);
    Debug(std::format("a (orthogonal direction): {:.6f} Angstroms", a), 1);
    Debug(std::format("Calculated in-plane repeat x (hypotenuse): {:.6f} Angstroms", x), 1);

    // Step 3: Construct orthorhombic lattice
    std::array<std::array<double, 3>, 3> superLattice = {{
        {x, 0.0, 0.0},  // X: in-plane repeat
        {0.0, 10, 0.0}, // Y: orthogonal
        {0.0, 0.0, 10}  // Z: depth in <hjk>
    }};

    Debug("--- Orthorhombic Supercell Lattice Vectors ---", 1);
    Debug(printMatrix(superLattice), 1);

    // Step 4: Populate BCC atoms
    std::map<int, Atom> atoms;
    int id = 1;
    for (int i = 0; i * a < x; ++i)
    {
        for (int j = 0; j * a < a; ++j)
        {
            for (int k = 0; k * a < na; ++k)
            {
                atoms[id++] = Atom("Fe", Vector3D(i * a, j * a, k * a));

                double cx = i * a + 0.5 * a;
                double cy = j * a + 0.5 * a;
                double cz = k * a + 0.5 * a;

                if (cx < x && cy < a && cz < na)
                {
                    atoms[id++] = Atom("Fe", Vector3D(cx, cy, cz));
                }
            }
        }
    }

    // Step 5: Write structure
    CrystalStructure structure(superLattice, atoms);
    FileWriter writer;
    writer.writeToFile("output/POSCAR", "VASP", structure);
    Debug("Slab with BCC atoms written to output/surface_POSCAR", 1);
}
