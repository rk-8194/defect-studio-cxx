#include "Command.h"

void Copy::execute(CommandArguments &args)
{
    // Must have FROM and TO defined.
    if (!args.hasArgument("FROM") || !args.hasArgument("TO"))
    {
        Debug("Must specify FROM and TO when using the COPY command.", -1);
    }

    // Source and target file paths.
    string source, target;
    source = args.findArgument("FROM")[0];
    target = args.findArgument("TO")[0];

    try
    {
        // Check if the source file exists
        if (!filesystem::exists(source))
        {
            Debug(format("Source file does not exist: {}.", source), -1);
            return;
        }

        // Check if the target directroy exists
        filesystem::path dirPath(target);
        filesystem::path parentDir = dirPath.parent_path();

        // Check if the parent directory exists
        if (!filesystem::exists(parentDir))
        {
            // Create the directory (and any necessary parent directories)
            filesystem::create_directories(parentDir);
            Debug(format("Created directory: {}", parentDir.string()), 0);
        }

        // Copy the file
        filesystem::copy(source, target, filesystem::copy_options::overwrite_existing);

        Debug(format("Copied file from {} to {}.", source, target), 0);
    }
    catch (const filesystem::filesystem_error &e)
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
