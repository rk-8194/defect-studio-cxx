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
    g_verbosity = 2;

    // Create the file readers.
    FileReader tasksReader("TASKS");

    // Read the TASKS file.
    TaskManager tasks(tasksReader);

    return 0;
}
