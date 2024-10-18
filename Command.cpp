#include "Command.h"

Command::Command()
{
}

#pragma region Private Functions

void Command::checkInputFile(CommandArguments &args)
{
    if (args.hasArgument("INPUT_FILE"))
    {
        std::vector<std::string> inputFiles = args.findArgument("INPUT_FILE");

        if (!inputFiles.empty())
        {
            std::string path = inputFiles[0]; // Safely access the first element
            FileReader reader(path);          // Set the work structure.
        }
        else
        {
            Debug("INPUT_FILE argument was found but no input file provided!", -1);
        }
    }
}

#pragma endregion

void Command::testCommand(CommandArguments &args)
{
    dsutil::Debug("This is a test command", 0);
    args.printArguments();
}

void Command::recenter(CommandArguments &args)
{
    map<int, Atom> atoms = g_workStructure.getAtoms();
}

void Command::substitute(CommandArguments &args)
{
    args.printArguments();

    // Check for a new input file. Set the work structure if a new input file is loaded.
    checkInputFile(args);

    // Get the list of atoms in the current working structure.
    map<int, Atom> atoms = g_workStructure.getAtoms();

    // Get the required arguments for the substitute task.
    string oldElement;
    string newElement;
    int amount, percent, fraction;

    // ... the element to target in the current cell.
    if (args.hasArgument("FROM"))
        oldElement = args.findArgument("FROM")[0];

    // ... the element that will replace the target element.
    if (args.hasArgument("TO"))
        oldElement = args.findArgument("TO")[0];

    if (!args.hasArgument("AMOUNT") && !args.hasArgument("FRACTION") && args.hasArgument("PERCENT"))
    {
        Debug("You must specify a number, fraction or percentage of atoms to substitute!", -1);
        return;
    }

    // ... the raw number of atoms to replace.
    if (args.hasArgument("AMOUNT"))
        amount = stoi(args.findArgument("AMOUNT")[0]);

    // ... the fraction of atoms to replace.
    if (args.hasArgument("FRACTION"))
        fraction = stoi(args.findArgument("FRACTION")[0]);

    // ... the percentage of atoms to replace.
    if (args.hasArgument("PERCENT"))
        percent = stoi(args.findArgument("PERCENT")[0]);

    // If a fraction has been specified, convert this to a number.
    if (args.hasArgument("FRACTION"))
        amount = static_cast<int>(floor(fraction * atoms.size()));

    // If a fraction has been specified, convert this to a number.
    if (args.hasArgument("PERCENT"))
        amount = static_cast<int>(floor((percent / 100) * atoms.size()));
}
