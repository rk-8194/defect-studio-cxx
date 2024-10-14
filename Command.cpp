#include "Command.h"

Command::Command()
{
}

void Command::testCommand(CommandArguments &args)
{
    dsutil::Debug("This is a test command", 0);
    args.printArguments();
}

void Command::recenter(CommandArguments &args)
{
    map<int, Atom> atoms = g_workStructure.getAtoms();
}
