#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class FileReader
{
  public:
    FileReader(const string &path);                    // Constructor declaration
    FileReader(const string &path, const int &format); // Constructor declaration (with file format specified).
    string getFilePath();
    int getFileFormat();
    string getLine(const int &index);
    int getFileLength();

  private:
    vector<string> file; // A store of the current file.
    string filePath;     // The path which points to the file.
    int fileFormat; // The file format which is either specified by the user, detected by the extension, or detected
                    // by the program.

    void readFile(const string &path);
    void addLineToFile(const string &newLine);
};
