#pragma once

#include "CrystalStructure.h"

#include <map>

// Debug
extern bool g_debugMode;
extern int g_verbosity;

// User defined variables.
extern map<string, string> g_userDefined;

// Task manager
extern int g_randomIDCount;

// Crystal structure
extern CrystalStructure g_workStructure;

// Tasks
extern int g_maxSubAttempts;
extern int g_maxIterations;
extern int g_currentIteration;
