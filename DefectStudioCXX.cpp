// DefectStudioCXX.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "CrystalStructure.h"
#include "FileReader.h"
#include "global.h"
#include <iostream>

int main()
{
    std::cout << "Hello World!\n";

    g_debugMode = true;
    g_verbosity = 1;

    FileReader reader("POSCAR");

    CrystalStructure structure;
    structure.loadFromFileReader(reader);

    return 0;
}
