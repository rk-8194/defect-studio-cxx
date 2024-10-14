#include "FileReader.h"
#include "dsutil.h"
#include "global.h"
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace dsutil;
using namespace std;

/// <summary>
/// Adds a line to the 'file' vector for access later.
/// </summary>
/// <param name="newLine"></param>
void FileReader::addLineToFile(const string &newLine)
{
    file.push_back(newLine);

    // Debug message (full)
    if (g_debugMode && g_verbosity >= 3)
        Debug(format("[Added line: {}", newLine), 3);
}

// Default constructor. Does not read anything.
FileReader::FileReader()
{
    fileFormat = static_cast<int>(-1);
}

/// <summary>
/// Constructor implementation for the file reader.
/// </summary>
/// <param name="path"> - The path to the file, relative to the directory the program is run from.</param>
FileReader::FileReader(const string &path)
{
    // Constructor debug message (detailed).
    if (g_debugMode && g_verbosity >= 2)
        Debug(format("Created a FileReader object. Reading from file: {}", path), 2);

    filePath = path;
    readFile(path);
}

/// <summary>
/// Constructor implementation for the file reader, specifying the file format.
/// </summary>
/// <param name="path"> - The path to the file relative to the directory that the program is run in.</param>
/// <param name="format"> - The format of the file.</param>
FileReader::FileReader(const string &path, const int &format)
{
    // Debug message (detailed).
    Debug(std::format("Created a FileReader object. Reading from file: {}", path), 2);

    filePath = path;
    fileFormat = format;
    readFile(path);
}

void FileReader::readFile(const string &path)
{
    // Debug message (standard).
    Debug(std::format("Reading from file: {}", path), 1);

    // Create an input file stream (ifstream). Open the file in binary mode.
    ifstream inputFile(path);

    // Check if the file is open.
    if (!inputFile.is_open())
    {
        // Debug message (Error/Warning)
        Debug("Error: Could not open the file!", -1);
        return;
    }

    string line;
    int lineNumber = 0;

    // Read file line-by-line using getline
    while (getline(inputFile, line))
    {
        addLineToFile(line);
        lineNumber++;

        // Debug message (standard)
        Debug(format("Line {} \t {}", lineNumber, line), 2);
    }

    // Close the file after reading
    inputFile.close();

    // Final debug messages.
    Debug(format("Finished reading from path: {}", path), 2);
    Debug(format("File is of type: {}", getFileFormat()), 1);
}

int FileReader::getFileFormat()
{
    // Attempt to detect VASP input by either the file name or the file extension.
    // Looking for: 'POSCAR', 'CONTCAR' or '*.vasp'
    if (filePath == "POSCAR" || filePath == "CONTCAR" ||
        (filePath.length() > 5 && filePath.substr(filePath.length() - 5) == ".vasp"))
    {
        return formatMap["VASP"];
    }
    else
    {
        return 0;
    }
}

string FileReader::getFilePath()
{
    return filePath;
}

string FileReader::getLine(const int &index)
{
    return file[index];
}

int FileReader::getFileLength()
{
    return static_cast<int>(file.size());
}
