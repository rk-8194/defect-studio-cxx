#include "TaskManager.h"

using namespace dsutil;
using namespace std;

// Definition of registered commands.
unordered_map<string, function<void(CommandArguments &)>> TaskManager::registeredCommands = {
    {"SET",
     [](CommandArguments &args) {
         auto cmd = std::make_unique<SetGlobals>();
         cmd->execute(args);
     }},
    {"TIS",
     [](CommandArguments &args) {
         auto cmd = std::make_unique<TetrahedralInterstitial>();
         cmd->execute(args);
     }},
    {"COPY",
     [](CommandArguments &args) {
         auto cmd = std::make_unique<Copy>();
         cmd->execute(args);
     }},
    {"RECENTER",
     [](CommandArguments &args) {
         auto cmd = std::make_unique<Recenter>();
         cmd->execute(args);
     }},
    {"VOLUME",
     [](CommandArguments &args) {
         auto cmd = std::make_unique<Volume>();
         cmd->execute(args);
     }},
    {"VACANCY",
     [](CommandArguments &args) {
         auto cmd = std::make_unique<Vacancy>();
         cmd->execute(args);
     }},
    {"SUBSTITUTE", [](CommandArguments &args) {
         auto cmd = std::make_unique<Substitute>();
         cmd->execute(args);
     }}};

// Definition of registered arguments.
unordered_map<string, int> TaskManager::registeredArguments = {
    {"INPUT_FILE", 1}, {"OUTPUT_DIR", 1}, {"OUTPUT_FILE", 1}, {"POSITION", 3}, {"ROTATION", 3}, {"FROM", 1},
    {"TO", 1},         {"AMOUNT", 1},     {"FRACTION", 1},    {"PERCENT", 1},  {"AMOUNT", 1},   {"STEPS", 1},
    {"REPEAT", 1},     {"MIN", 1},        {"MAX", 1},         {"DIM", 1},      {"ELEMENT", 1},  {"BRAVAIS", 1},
    {"CLUSTER", 1},    {"ALL", 0},        {"DELETE", 0}};

// Default constructor.
TaskManager::TaskManager()
{
    // Initializes a blank file reader.
    managerID = randomID();
    Debug(format("Assigned manager ID {}.", managerID), 3);
}

// New task manager using the designated file reader.
TaskManager::TaskManager(FileReader &reader) : fileReader(reader)
{
    if (reader.getFileLength() <= 0)
    {
        Debug("No task file found", -1);
        managerID = randomID();
        return;
    }

    // Assign a manager ID.
    managerID = randomID();
    Debug(format("Assigned manager ID {}.", managerID), 3);

    readTasks();
}

// Sets the parent of the current task.
void TaskManager::setParent(TaskManager &parent)
{
    parentManager = make_shared<TaskManager>(parent); // Create a shared pointer to parent
    Debug(format("Task manager {} is child of manager {}", managerID, parent.managerID), 3);
}

void TaskManager::readTasks()
{
    // Read tasks from the file and execute commands
    for (int i = 0; i < fileReader.getFileLength(); ++i)
    {
        // Get the whole line.
        std::string currentLine = fileReader.getLine(i);

        // Skip blank lines by checking if the line is empty after trimming whitespace.
        if (currentLine.empty() || std::all_of(currentLine.begin(), currentLine.end(), ::isspace))
            continue;

        // Get the whole line as a stringstream.
        stringstream line(fileReader.getLine(i));

        // Put the line into an array.
        vector<string> keys;
        string key;
        while (line >> key)
        {
            keys.push_back(key);
        }

        // Use the first key as the command.
        string command = keys[0];

        // Skip comment lines.
        if (command == "#" || keys.size() == 0)
            continue;

        // Go through each remaining key and look for argument keywords.
        CommandArguments args;

        vector<int> indexToSkip;
        for (int i = 0; i < keys.size(); ++i)
        {
            // Check if the current index is marked to be skipped
            if (find(indexToSkip.begin(), indexToSkip.end(), i) != indexToSkip.end())
                continue;

            // Check if the current key is a registered argument keyword
            auto arg = registeredArguments.find(keys[i]);
            if (arg != registeredArguments.end())
            {
                int maxValues = registeredArguments.at(keys[i]); // Get expected number of values for this argument
                vector<string> values;

                // Extract argument values based on maxValues
                for (int j = 0; j < maxValues; ++j)
                {
                    // Ensure we don't run out of bounds while accessing keys
                    if (i + j + 1 >= keys.size())
                    {
                        Debug("Missing argument - end of task line.", -1);
                        return;
                    }

                    // Check if the next key is not another argument keyword
                    if (registeredArguments.find(keys[i + j + 1]) != registeredArguments.end())
                    {
                        Debug("Argument keyword found in values list!", -1);
                        return;
                    }

                    // Add the current index to the skip list and add the value to the argument list
                    indexToSkip.push_back(i + j + 1);
                    values.push_back(keys[i + j + 1]); // Safely add value
                }

                // Check if we have the exact number of expected values
                if (static_cast<int>(values.size()) != maxValues)
                {
                    Debug("Missing argument in task line!", -1);
                    return;
                }

                // Add the argument and its associated values to the args object
                args.addArgument(keys[i], values);
            }
        }

        executeTask(keys[0], args);
    }
}

void TaskManager::executeTask(const string &commandName, CommandArguments &arguments)
{
    // Find the command in the registered commands map
    auto it = registeredCommands.find(commandName);

    if (it != registeredCommands.end())
    {
        // Check if the "REPEAT" argument is present
        if (arguments.hasArgument("REPEAT"))
        {
            // Get the number of iterations for repeat
            g_maxIterations = stoi(arguments.findArgument("REPEAT")[0]);

            for (int i = 0; i < g_maxIterations; ++i)
            {
                // Update the current iteration counter
                g_currentIteration = i + 1;

                // Execute the command (using the lambda from the map)
                it->second(arguments); // This lambda is responsible for creating and executing the command
            }

            // Reset the iteration count
            g_currentIteration = 0;
        }
        else
        {
            // If "REPEAT" argument isn't present, just execute the command once
            it->second(arguments); // This lambda is responsible for creating and executing the command
        }
    }
    else
    {
        // If the command isn't found, output an error message
        Debug(format("Invalid command: {}", commandName), -1);
    }
}
