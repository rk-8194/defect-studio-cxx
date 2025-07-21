#include "Command.h"

#pragma region Protected Functions

/// <summary>
/// Checks that the given list of arguments has all of the required arguments, and prints out an error message if this
/// is not the case.
/// </summary>
/// <param name="args">- The list of arguments passed to the command.</param>
/// <param name="required">- The required argument keywords.</param>
/// <param name="errorMessage">- The error message to print if a keyword is missing.</param>
/// <returns></returns>
bool Command::hasArguments(CommandArguments &args, const vector<string> &required, const string &errorMessage)
{
    for (int i = 0; i < required.size(); ++i)
    {
        if (!args.hasArgument(required[i]))
        {
            Debug(errorMessage, -1);
            return false;
        }
    }

    return true;
}

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
