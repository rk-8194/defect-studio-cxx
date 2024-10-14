#include "Command.h"

Command::Command()
{
}

void Command::testCommand(CommandArguments &args)
{
    dsutil::Debug("This is a test command", 0);
    args.printArguments();
}
