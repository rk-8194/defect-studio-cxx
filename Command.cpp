#include "Command.h"

#pragma region Protected Functions
/// <summary>
/// Checks to see if an input file has been specified.
/// If true, open the input file.
/// </summary>
/// <param name="args">- The list of arguments passed to the command.</param>
void Command::checkInputFile(CommandArguments &args)
{
    if (args.hasArgument("INPUT_FILE"))
    {
        std::vector<std::string> inputFiles = args.findArgument("INPUT_FILE");

        if (!inputFiles.empty())
        {
            std::string path = inputFiles[0]; // Safely access the first element
            FileReader reader(path, 1);       // Set the work structure.
        }
        else
        {
            Debug("INPUT_FILE argument was found but no input file provided!", -1);
        }
    }
}

/// <summary>
/// Checks to see if an output directory has been specified.
/// </summary>
/// <param name="args">- The list of arguments passed to the command.</param>
void Command::checkOutputDirectory(CommandArguments &args)
{
    if (args.hasArgument("OUTPUT_DIR"))
    {
        // Get the target file path.
        std::stringstream path(args.findArgument("OUTPUT_DIR")[0]);

        std::string segment;
        std::vector<std::string> segments;

        while (std::getline(path, segment, '/'))
        {
            segments.push_back(segment);
        }
    }
}

#pragma endregion

/// <summary>
/// Executes a given command.
/// </summary>
/// <param name="args">- The list of arguments passed to the command.</param>
void Command::execute(CommandArguments &args)
{
    // Default implementation. Does nothing by itself.
}
