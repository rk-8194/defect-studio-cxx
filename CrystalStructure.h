#pragma once

#include "Atom.h"
#include "FileReader.h"
#include <array>
#include <iostream>

class CrystalStructure
{
  public:
    CrystalStructure();
    void loadFromFileReader(FileReader &fileReader);
    void printStructure();
    void addAtom(const int &index, const Atom &atom);
    map<int, Atom> getAtoms();
    std::map<int, Atom> getAtomsOfType(const string &type);
    void replaceAtom(const int& index, const Atom& atom);

  private:
    map<int, Atom> atoms;
    array<array<double, 3>, 3> lattice;
    map<string, int> speciesMap;
    map<string, int> cumulativeSpeciesMap;
    void buildVasp(FileReader &fileReader);
};
