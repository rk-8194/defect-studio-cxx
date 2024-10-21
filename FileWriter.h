#pragma once
#include "dsutil.h"
#include "global.h"
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>

class FileWriter
{
  public:
    FileWriter();
    void writeToFile(const string &path, const string &format);
    void writeToFile(const string &path, const string &fileFormat, const map<int, Atom> &atoms,
                     const array<array<double, 3>, 3> &lattice);
    void writeToFile(const string &path, const string &format, const CrystalStructure &structure);

  private:
    void writeVASP(ofstream &outfile, CrystalStructure structure);
};
