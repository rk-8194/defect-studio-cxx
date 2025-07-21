#include "Command.h"

void SetGlobals::execute(CommandArguments &args)
{
    if (args.hasArgument("MAX_SUB"))
    {
        g_maxSubAttempts = stoi(args.findArgument("MAX_SUB")[0]);
        Debug(format("SET \tMaximum substitution attempts\n\t\tValue: {}", g_maxSubAttempts), 0);
    }
}
