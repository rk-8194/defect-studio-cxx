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

    // Debug settings. Print debug messages at verbosity level 3.
    g_debugMode = true;
    g_verbosity = 3;

    // Create the file reader to read from the TASKS file.
    FileReader tasksReader("TASKS");

    // Read the TASKS file and run the tasks.
    TaskManager tasks(tasksReader);

    return 0;
}
