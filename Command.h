#pragma once
#include "CommandArguments.h"
#include "CrystalStructure.h"
#include "dsutil.h"
#include "global.h"
#include <string>

class Command
{
  public:
    Command();
    void testCommand(CommandArguments &args);
    void recenter(CommandArguments &args);
};
