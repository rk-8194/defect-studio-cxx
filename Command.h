#pragma once
#include "CommandArguments.h"
#include "CrystalStructure.h"
#include "FileWriter.h"
#include "dsutil.h"
#include "global.h"
#include <algorithm>
#include <iterator>
#include <random>
#include <string>

class Command
{
  private:
    void checkInputFile(CommandArguments &args);

  public:
    Command();
    void testCommand(CommandArguments &args);
    void setGlobals(CommandArguments& args);
    void recenter(CommandArguments &args);
    void substitute(CommandArguments &args);
    void volume(CommandArguments &args);
};
