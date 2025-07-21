#pragma once

#include "Command.h"
#include "CommandArguments.h"
#include "FileReader.h"
#include "dsutil.h"
#include "global.h"
#include <format>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class TaskManager
{
  public:
    TaskManager();
    TaskManager(FileReader &reader);

    int managerID;
    std::shared_ptr<TaskManager> parentManager = nullptr;
    FileReader fileReader;

    void setParent(TaskManager &parent);
    void executeTask(const std::string &commandName, CommandArguments &arguments);

  private:
    // Initialise the commands and arguments that can be used in the TASKS file.
    static unordered_map<string, function<void(CommandArguments &)>> registeredCommands;
    static unordered_map<string, function<void(const std::string &commandName)>> commandMap;
    static unordered_map<string, int> registeredArguments;

    void readTasks();
};
