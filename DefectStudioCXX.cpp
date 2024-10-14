// DefectStudioCXX.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "CrystalStructure.h"
#include "FileReader.h"
#include "TaskManager.h"
#include "global.h"
#include <iostream>

int main()
{
    // Initialisation message.
    std::cout << "Hello World!\n";

    // Debug settings.
    g_debugMode = true;
    g_verbosity = 3;

    // Create the file readers.
    FileReader reader("POSCAR");
    FileReader tasksReader("TASKS");
    FileReader childTasks("TASKS");

    // Load the crystal structure.
    CrystalStructure structure;
    structure.loadFromFileReader(reader);

    // Read the TASKS file.
    TaskManager tasks(tasksReader);

    return 0;
}
