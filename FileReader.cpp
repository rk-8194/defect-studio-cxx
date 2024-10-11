#include "FileReader.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/// <summary>
/// Adds a line to the 'file' vector for access later.
/// </summary>
/// <param name="newLine"></param>
void FileReader::addLineToFile(const string &newLine)
{
    file.push_back(newLine);
    cout << "Adding line: " << newLine << endl;
}

/// <summary>
/// Constructor implementation for the file reader.
/// </summary>
/// <param name="path"> - The path to the file, relative to the directory the program is run from.</param>
FileReader::FileReader(const string &path)
{
    filePath = path;
    cout << "Created a FileReader object. Reading from file:" << path << endl;
    readFile(path);
}

/// <summary>
/// Constructor implementation for the file reader, specifying the file format.
/// </summary>
/// <param name="path"> - The path to the file relative to the directory that the program is run in.</param>
/// <param name="format"> - The format of the file.</param>
FileReader::FileReader(const string &path, const string &format)
{
    filePath = path;
    fileFormat = format;
    cout << "Created a FileReader object. Reading from file:" << path << endl;
    readFile(path);
}

void FileReader::readFile(const string &path)
{
    cout << "Reading from path: " << path << endl;

    // Create an input file stream (ifstream). Open the file in binary mode.
    ifstream inputFile(path);

    // Check if the file is open.
    if (!inputFile.is_open())
    {
        cout << "Error: Could not open the file!" << endl;
        return;
    }

    string line;
    int lineNumber = 0;

    // Read file line-by-line using getline
    while (getline(inputFile, line))
    {
        cout << lineNumber << ":" << line << endl;
        addLineToFile(line);
        lineNumber++;
    }

    // Close the file after reading
    inputFile.close();

    cout << "Finished reading from path: " << path << endl;

    cout << "File is of type: " << getFileFormat() << endl;
}

string FileReader::getFileFormat()
{
    // Attempt to detect VASP input by either the file name or the file extension.
    // Looking for: 'POSCAR', 'CONTCAR' or '*.vasp'
    if (filePath == "POSCAR" || filePath == "CONTCAR" ||
        (filePath.length() > 5 && filePath.substr(filePath.length() - 5) == ".vasp"))
    {
        return "VASP";
    }
    else
    {
        return "???";
    }
}
