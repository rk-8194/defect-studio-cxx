#pragma once
#include "CommandArguments.h"
#include "CrystalStructure.h"
#include "dsutil.h"
#include "global.h"
#include <string>

class Command
{
  private:
    void checkInputFile(CommandArguments &args);

  public:
    Command();
    void testCommand(CommandArguments &args);
    void recenter(CommandArguments &args);
    void substitute(CommandArguments &args);
};
