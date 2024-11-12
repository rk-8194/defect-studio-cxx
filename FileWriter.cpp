#include "FileWriter.h"

using namespace std;
using namespace dsutil;

// Default constructor.
FileWriter::FileWriter()
{
}

#include <filesystem>
#include <iostream>
#include <sstream>
#include <vector>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <vector>

string FileWriter::verifyPath(const std::string &path, int currentIteration)
{
    Debug(format("Verifying path: {}", path), 3);

    // Split path into directory and file
    size_t lastSlash = path.find_last_of('/');
    std::string directoryPath = (lastSlash == std::string::npos) ? "" : path.substr(0, lastSlash);
    std::string fileName = (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);

    std::stringstream pathstream(directoryPath);
    std::string directory;
    std::vector<std::string> directories;

    // Split the directory path by '/'
    while (std::getline(pathstream, directory, '/'))
    {
        directories.push_back(directory);
    }

    if (directories.size() == 0 && g_maxIterations <= 1)
        return path;

    std::string currentDirectory = "";
    for (int i = 0; i < directories.size(); ++i)
    {
        std::string targetDirectory;
        if (i > 0)
            targetDirectory = currentDirectory + "/" + directories[i];
        else
            targetDirectory = directories[0];

        Debug(format("Verifying directory: {}", targetDirectory), 3);

        try
        {
            if (std::filesystem::exists(targetDirectory))
            {
                if (std::filesystem::is_regular_file(targetDirectory))
                {
                    throw std::runtime_error(
                        format("A file with the name '{}' exists where a directory is expected.", targetDirectory));
                }
            }
            else
            {
                Debug(format("Directory not found. Creating directory: {}", targetDirectory), 3);
                std::filesystem::create_directories(targetDirectory);
            }
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
            return path;
        }
        catch (const std::runtime_error &e)
        {
            std::cerr << "Runtime error: " << e.what() << std::endl;
            return path;
        }

        currentDirectory = targetDirectory;
    }

    // If multiple iterations are used, create a subdirectory for the current iteration.
    if (g_maxIterations > 1)
    {
        std::string iterationDirectory;
        if (directories.size() > 1)
            iterationDirectory = currentDirectory + "/" + std::to_string(currentIteration);
        else
            iterationDirectory = std::to_string(currentIteration);
        Debug(format("Creating iteration directory: {}", iterationDirectory), 3);

        try
        {
            if (!std::filesystem::exists(iterationDirectory))
            {
                std::filesystem::create_directory(iterationDirectory);
            }
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
            return path;
        }

        currentDirectory = iterationDirectory; // Set the current directory to the iteration folder.
    }

    // Reconstruct the full path by combining currentDirectory with the original file name
    std::string fullPath = currentDirectory + "/" + fileName;
    Debug(format("Final verified path for file: {}", fullPath), 3);

    return fullPath;
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
    string formattedPath = path;

    // Verify the path.
    formattedPath = verifyPath(formattedPath, g_currentIteration);

    ofstream outfile(formattedPath);

    // Print error if file could not be opened.
    if (!outfile)
    {
        Debug(format("Could not open file at path: {}", formattedPath), -1);
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
