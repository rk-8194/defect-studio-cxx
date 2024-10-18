#include "CrystalStructure.h"
#include "global.h"
#include <algorithm>
#include <format>
#include <sstream>

using namespace dsutil;

/// <summary>
/// Default constructor for the crystal structure.
/// </summary>
CrystalStructure::CrystalStructure()
{
    lattice = {};
    atoms = {};

    // Debug message (detailed)
    Debug("Created new (empty) crystal structure", 2);
}

void CrystalStructure::loadFromFileReader(FileReader &fileReader)
{
    // Debug message (standard)
    Debug(std::format("Reading crystal structure from {} ... ", fileReader.getFilePath()), 1);

    // Change the reader depending on what the file format is.
    Debug(std::format("File format: {}", fileReader.getFileFormat()), 2);
    void (CrystalStructure::*buildStructure)(FileReader &) = nullptr; //

    switch (fileReader.getFileFormat())
    {
    case 1:
        buildStructure = &CrystalStructure::buildVasp;
        break;
    default:
        Debug("Invalid file format was detected. Could not build structure", 0);
    }

    // Builds the structure according to the format which has been identified.
    if (buildStructure)
    {
        (this->*buildStructure)(fileReader);
    }

    g_workStructure = *this;
}

std::map<int, Atom> CrystalStructure::getAtoms()
{
    return atoms;
}

std::map<int, Atom> CrystalStructure::getAtomsOfType(const string &type)
{
    Debug(format("Getting atoms of type: {}", type), 3);

    std::map<int, Atom> result; // New map to hold the filtered results

    // Iterate over the original map
    for (const auto &pair : atoms)
    {
        const Atom &atom = pair.second; // Access the Atom object

        // If the atomType matches, insert the Atom into the result map
        if (atom.atomType == type)
        {
            result.insert(pair); // Insert the whole pair (key and Atom)
        }
    }

    Debug(format("Number of atoms in filtered atom list: {}", result.size()), 3);

    return result; // Return the filtered map
}

void CrystalStructure::replaceAtom(const int &index, const Atom &atom)
{
    Debug(format("Atom at position {} was substituted.\n\t\tPrevious: {} \t| New: {}", index, atoms[index].atomType,
                 atom.atomType),
          1);
    atoms[index] = atom;
}

#include <algorithm> // Required for std::sort
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

void CrystalStructure::buildVasp(FileReader &fileReader)
{
    Debug("Building crystal structure from VASP input file ...", 1);

    // BUILT-IN list of lines and their meanings.
    map<string, int> poscar = {{"COMMENT", 0},         {"SCALING_FACTOR", 1},    {"LATTICE_XN", 2},
                               {"LATTICE_YN", 3},      {"LATTICE_ZN", 4},        {"SPECIES_LABELS", 5},
                               {"SPECIES_NUMBERS", 6}, {"COORDINATE_SYSTEM", 7}, {"COORDINATES_START", 8}};

    // Get the lattice parameters.
    std::stringstream xn(fileReader.getLine(poscar["LATTICE_XN"]));
    std::stringstream yn(fileReader.getLine(poscar["LATTICE_YN"]));
    std::stringstream zn(fileReader.getLine(poscar["LATTICE_ZN"]));

    for (int i = 0; i < 3; ++i)
    {
        xn >> lattice[0][i];
        yn >> lattice[1][i];
        zn >> lattice[2][i];
    }

    // Scale all elements of the lattice by the scaling factor.
    std::stringstream scaling(fileReader.getLine(poscar["SCALING_FACTOR"]));
    double scalingFactor;
    scaling >> scalingFactor;

    for (int m = 0; m < 3; ++m)
    {
        for (int n = 0; n < 3; ++n)
        {
            lattice[m][n] = lattice[m][n] * scalingFactor;
        }
    }

    // Debug message (detailed)
    Debug(printMatrix(lattice), 0);

    // Get the map of atoms and species.
    std::stringstream labels(fileReader.getLine(poscar["SPECIES_LABELS"]));
    std::stringstream counts(fileReader.getLine(poscar["SPECIES_NUMBERS"]));

    string label;
    int count;
    int total = 0;

    // Create a vector to store species and counts in the correct order (based on file).
    vector<pair<string, int>> speciesList;

    while (labels >> label && counts >> count)
    {
        speciesList.push_back({label, count}); // Store species and count pairs
    }

    // Now, we will populate the speciesMap and cumulativeSpeciesMap.
    total = 0;
    for (const auto &species : speciesList)
    {
        speciesMap[species.first] = species.second;
        total += species.second;
        cumulativeSpeciesMap[species.first] = total;
    }

    // Get the positions of all atoms and associate them with the species.
    int atomIndex = 0; // Start atom index at 1, but the loop uses 0-based indexing
    for (int j = poscar["COORDINATES_START"]; j < fileReader.getFileLength(); ++j)
    {
        std::stringstream pos(fileReader.getLine(j));
        string line = pos.str();

        if (line.empty())
        {
            Debug(format("Reached end of coordinates. Atoms read: {}", j - poscar["COORDINATES_START"]), 3);
            break;
        }
        else
        {
            Debug(format("Adding line: {}", line), 3);
        }

        // Convert the position into an array.
        array<double, 3> position;
        for (int p = 0; p < 3; ++p)
        {
            pos >> position[p];
        }

        // Increment atom index to determine which species to assign.
        atomIndex++; // increment atom index to start at 1 for the first atom

        // Variables for cumulative range checking
        int previousTotal = 0;
        std::string _element;

        // Find the species based on cumulative count range
        for (const auto &pair : speciesList) // Use speciesList instead of cumulativeSpeciesMap
        {
            // Check if the current atom's index falls within the cumulative range for a species.
            if (atomIndex > previousTotal && atomIndex <= previousTotal + pair.second)
            {
                _element = pair.first; // Found the species for this atom.
                break;
            }
            previousTotal += pair.second; // Update previous total for next iteration.
        }

        // Add the atom to the structure.
        Atom atom = Atom(_element, Vector3D(position[0], position[1], position[2]));
        addAtom(atomIndex, atom); // Assuming `addAtom` takes an atom index and atom object.
    }
}

void CrystalStructure::printStructure()
{
    // Only print the output in DETAILED mode.
    int _verbosity = 2;

    // Print the lattice parameters.
    Debug("---### Lattice Parameters ###---", _verbosity);
    Debug(format("{}", dsutil::printMatrix(lattice)), _verbosity);

    // Print the atoms.
    Debug("---### Atomic Coordinates ###---", _verbosity);
    for (int i = 0; i < atoms.size(); ++i)
    {
        Atom atom = atoms[i];
        Debug(format("{}\t{}\t{}\t{}\t{}", (i + 1), atom.atomType, atom.atomPosition.x, atom.atomPosition.y,
                     atom.atomPosition.z),
              _verbosity);
    }
}

void CrystalStructure::addAtom(const int &index, const Atom &atom)
{
    atoms[index] = atom;
    Debug(format("{}\t{}\t{}\t{}\t{}", index, atom.atomType, atom.atomPosition.x, atom.atomPosition.y,
                 atom.atomPosition.z),
          0);
}
