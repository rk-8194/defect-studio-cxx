#include "FileWriter.h"

using namespace std;
using namespace dsutil;

// Default constructor.
FileWriter::FileWriter()
{
}

// Writes the current working file to the path.
void FileWriter::writeToFile(const string &path, const string &fileFormat)
{
    writeToFile(path, fileFormat, g_workStructure);
}

// Writes the given atoms and lattice to an input file. Useful for when the work structure is not changed.
void FileWriter::writeToFile(const string &path, const string &fileFormat, const map<int, Atom> &atoms,
                             const array<array<double, 3>, 3> &lattice)
{
    CrystalStructure structure(lattice, atoms);
    writeToFile(path, fileFormat, structure);
}

// Write the selected structure file to the path.
void FileWriter::writeToFile(const string &path, const string &fileFormat, const CrystalStructure &structure)
{
    // Create an output file stream.
    string formattedPath;

    if (g_maxIterations > 1)
        formattedPath = format("{}_{}", path, g_currentIteration);
    else
        formattedPath = path;

    ofstream outfile(formattedPath);

    // Print error if file could not be opened.
    if (!outfile)
    {
        Debug(format("Could not open file at path: {}", path), -1);
        return;
    }

    /* Get the format ID from dsutil::formatMap */
    int formatID = -1;

    // Is the given string a valid file format?
    auto it = formatMap.find(fileFormat);
    if (it != formatMap.end())
    {
        formatID = it->second;
    }
    else
    {
        Debug(format("Could not write file type: {}", fileFormat), -1);
        return;
    }

    switch (formatID)
    {
    case 1: {
        writeVASP(outfile, structure);
        break;
    }
    default: {
        break;
    }
    }

    // Close the file.
    outfile.close();
}

void FileWriter::writeVASP(ofstream &outfile, CrystalStructure structure)
{
    // Write the header line.
    outfile << "Created by DefectStudio(c) 2024" << endl;

    // Write the scaling factor line.
    outfile << "1.0" << endl;

    // Write the lattice paramters
    outfile << dsutil::printMatrix(structure.getLattice()) << endl;

    // Write the species lines and the coordinate system.
    map<int, Atom> atoms = structure.getAtoms();

    // Group atoms by species.
    map<string, vector<Atom>> speciesAtoms;
    for (const auto &atomPair : atoms) // Assume atoms is a map<int, Atom>
    {
        const Atom &atom = atomPair.second;
        speciesAtoms[atom.atomType].push_back(atom);
    }

    // Write the species.
    for (const auto &species : speciesAtoms)
    {
        outfile << " " << species.first;
    }
    outfile << endl;

    // Write the counts (number of atoms per species).
    for (const auto &species : speciesAtoms)
    {
        outfile << " " << species.second.size();
    }
    outfile << endl;

    outfile << "Direct" << endl;

    // Write the atomic coordinates grouped by species.
    for (const auto &species : speciesAtoms)
    {
        for (const auto &atom : species.second)
        {
            outfile << format("{} {} {}", atom.atomPosition.x, atom.atomPosition.y, atom.atomPosition.z) << endl;
        }
    }
}
