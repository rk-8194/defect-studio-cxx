#include "TaskManager.h"

using namespace dsutil;
using namespace std;

// Definition of registered commands.
unordered_map<string, function<void(Command &, CommandArguments &)>> TaskManager::registeredCommands = {
    {"TASK1", [](Command &cmd, CommandArguments &args) { cmd.testCommand(args); }},
    {"RECENTER", [](Command &cmd, CommandArguments &args) { cmd.recenter(args); }},
    {"VOLUME", [](Command &cmd, CommandArguments &args) { cmd.volume(args); }},
    {"SUBSTITUTE", [](Command &cmd, CommandArguments &args) { cmd.substitute(args); }}};

// Definition of registered arguments.
unordered_map<string, int> TaskManager::registeredArguments = {
    {"INPUT_FILE", 1}, {"OUTPUT_FILE", 1}, {"POSITION", 3}, {"ROTATION", 3}, {"FROM", 1}, {"TO", 1},
    {"AMOUNT", 1},     {"FRACTION", 1},    {"PERCENT", 1},  {"AMOUNT", 1},   {"STEPS", 1}};

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

        // Go through each remaining key and look for argument keywords.
        CommandArguments args;

        vector<int> indexToSkip;
        for (size_t i = 0; i < keys.size(); ++i)
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

        Command cmd;
        executeTask(keys[0], cmd, args);
    }
}

void TaskManager::executeTask(const string &commandName, Command &command, CommandArguments &arguments)
{
    auto it = registeredCommands.find(commandName);
    if (it != registeredCommands.end())
    {
        it->second(command, arguments); // Call the command function, passing the Command instance
    }
    else
    {
        Debug(format("Invalid command: {}", commandName), -1);
    }
}
