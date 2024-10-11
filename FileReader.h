#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class FileReader
{
  public:
    FileReader(const string &path);                       // Constructor declaration
    FileReader(const string &path, const string &format); // Constructor declaration (with file format specified).

  private:
    vector<string> file; // A store of the current file.
    string filePath;     // The path which points to the file.
    string fileFormat; // The file format which is either specified by the user, detected by the extension, or detected
                       // by the program.

    void readFile(const string &path);
    void addLineToFile(const string &newLine);
    string getFileFormat();
};
