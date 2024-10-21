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

void Command::setGlobals(CommandArguments &args)
{
    if (args.hasArgument("MAX_SUB"))
    {
        g_maxSubAttempts = stoi(args.findArgument("MAX_SUB")[0]);
        Debug(format("SET \tMaximum substitution attempts\n\t\tValue: {}", g_maxSubAttempts), 0);
    }
}

void Command::recenter(CommandArguments &args)
{
    map<int, Atom> atoms = g_workStructure.getAtoms();
}

void Command::volume(CommandArguments &args)
{
    /* args.printArguments(); */

    // Check for a new input file. Set the work structure if a new input file is loaded.
    checkInputFile(args);

    // Get the list of atoms in the current working structure.
    map<int, Atom> atoms = g_workStructure.getAtoms();
    array<array<double, 3>, 3> lattice = g_workStructure.getLattice();

    // Get the volume of the current cell.
    double eqVolume = dsutil::determinant3x3(lattice);
    Debug(format("Equilibrium volume of cell: {}", eqVolume), 1);

    // Check if the paramters exist.
    double amount = 0; // as a percentage
    int totalSteps = 0;

    if (!args.hasArgument("AMOUNT") || !args.hasArgument("STEPS"))
    {
        Debug("Must specify the values of arguments AMOUNT and STEPS when performing volume-energy setup.", -1);
        return;
    }

    // Get the parameters.
    amount = stod(args.findArgument("AMOUNT")[0]) / 100;
    totalSteps = stoi(args.findArgument("STEPS")[0]);

    // Get the list of target volumes.
    vector<double> targetVolumes;
    double stepSize = (amount * 0.5) / totalSteps;

    for (int i = 0; i < (totalSteps * 0.5); ++i)
    {
        double _posVolume = eqVolume * (1 + (i * stepSize));
        cout << "+/- " << _posVolume << endl;

        double _negVolume = eqVolume * (1 - (i * stepSize));
        targetVolumes.push_back(_posVolume);
        targetVolumes.push_back(_negVolume);
    }

    // Sort the targetVolumes vector.
    std::sort(targetVolumes.begin(), targetVolumes.end());

    // For each target volume, scale the cell appropriatley.
    for (int j = 0; j < targetVolumes.size(); ++j)
    {
        Debug(format("Processing volume: {}", targetVolumes[j]), 2);

        // Get the scaling factor for each matrix element.
        double difference = targetVolumes[j] - eqVolume;
        double scalingFactor = 1.0 + ((1.0 / 3.0) * (difference / eqVolume));

        // Apply the scaling factor to each element of the matrix.
        array<array<double, 3>, 3> newMatrix = {};

        for (int a = 0; a < lattice.size(); ++a)
        {
            for (int b = 0; b < lattice[a].size(); ++b)
            {
                cout << (scalingFactor)*lattice[a][b] << endl;
                FileWriter
            }
        }
    }
}

void Command::substitute(CommandArguments &args)
{
    // args.printArguments();

    // Check for a new input file. Set the work structure if a new input file is loaded.
    checkInputFile(args);

    // Get the desired output file name.
    string output = "OUTPUT";
    if (args.hasArgument("OUTPUT_FILE"))
        output = args.findArgument("OUTPUT_FILE")[0];

    // Get the list of atoms in the current working structure.
    map<int, Atom> atoms = g_workStructure.getAtoms();

    // Get the required arguments for the substitute task.
    string oldElement, newElement;
    int amount = 0;
    double percent = 0, fraction = 0;

    // ... the element to target in the current cell.
    if (args.hasArgument("FROM"))
        oldElement = args.findArgument("FROM")[0];

    // ... the element that will replace the target element.
    if (args.hasArgument("TO"))
        newElement = args.findArgument("TO")[0];

    if (!args.hasArgument("AMOUNT") && !args.hasArgument("FRACTION") && !args.hasArgument("PERCENT"))
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

    // Make a substitution of [AMOUNT] atoms.
    // Keep a list of atoms that have already been changed.
    Debug(format("Attempting to make {} substitutions of {} -> {}", amount, oldElement, newElement), 1);
    vector<int> excluded;
    for (int i = 0; i < amount; ++i)
    {
        // Get a list of atoms that CAN be substituted.
        // Cancel if there are not enough atoms in the cell.
        map<int, Atom> candidateAtoms = g_workStructure.getAtomsOfType(oldElement);
        if (candidateAtoms.empty() || candidateAtoms.size() < amount)
        {
            Debug("Not enough atoms in the cell to substitute!", -1);
            return;
        }

        // Create a random number generator
        std::random_device rd;                                               // Obtain a random number from hardware
        std::mt19937 gen(rd());                                              // Seed the generator
        std::uniform_int_distribution<> distr(0, candidateAtoms.size() - 1); // Define the range

        // Generate a random index that is not in the excluded vector
        int randomIndex;
        int attemptCount = 0; // Do not exceed the maximum number of substitution attempts.
        do
        {
            if (attemptCount > g_maxSubAttempts)
            {
                Debug("Maximum substitution attempts was exceeded.", -1);
                return;
            }

            std::uniform_int_distribution<> distr(0, candidateAtoms.size() - 1); // Define the range
            randomIndex = distr(gen);
            ++attemptCount;
        } while (std::find(excluded.begin(), excluded.end(), randomIndex) != excluded.end());

        // Move iterator to the random index
        auto it = candidateAtoms.begin();
        std::advance(it, randomIndex);

        // Create a new element with the same position but different type.
        // Added it to the current work structure by replacing the atom at the same index.
        g_workStructure.replaceAtom(it->first, Atom(newElement, it->second.atomPosition));
        excluded.push_back(it->first);
    }

    // Print the final structure to the terminal (depends on verbosity level)
    g_workStructure.printStructure();
    FileWriter writer;
    writer.writeToFile(output, "VASP", g_workStructure);
}
