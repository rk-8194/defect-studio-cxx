#include "Command.h"

void Volume::execute(CommandArguments &args)
{
    /* args.printArguments(); */

    // Break cases.
    if (!args.hasArgument("AMOUNT") || !args.hasArgument("STEPS"))
    {
        Debug("Must specify the values of arguments AMOUNT and STEPS when performing volume-energy setup.", -1);
        return;
    }

    // Check for a new input file. Set the work structure if a new input file is loaded.
    checkInputFile(args);

    // Check for an output directory. This function will make the required directories if necessary.
    checkOutputDirectory(args);

    // Get the list of atoms in the current working structure.
    map<int, Atom> atoms = g_workStructure.getAtoms();
    array<array<double, 3>, 3> lattice;

    /* Get the volume of the current cell. */
    // Has the user specified MIN/MAX/FRACTION? (only for cubic cells)
    //      - YES - Calculate the equilibrium volume according to Vegard's law.
    //      - NO  - Calculate the equilibrium volume according to the input file.
    double eqVolume = 0.0;
    if (args.hasArgument("MIN") && args.hasArgument("MAX") && args.hasArgument("FRACTION") && args.hasArgument("DIM"))
    {
        double minLattice = stod(args.findArgument("MIN")[0]);
        double maxLattice = stod(args.findArgument("MAX")[0]);
        double fraction = stod(args.findArgument("FRACTION")[0]);
        double dim = stod(args.findArgument("DIM")[0]);

        double vegardLattice = (1 - fraction) * maxLattice + (fraction * minLattice);
        Debug(format("Calculated lattice parameter according to Vegard's law: {}", vegardLattice), 1);
        lattice = {{{vegardLattice * dim, 0, 0}, {0, vegardLattice * dim, 0}, {0, 0, vegardLattice * dim}}};
        eqVolume = dsutil::determinant3x3(lattice);
        Debug(format("Calculating equilibrium volume of cell from Vegard's law: {}", eqVolume), 1);
    }
    else
    {
        lattice = g_workStructure.getLattice();
        eqVolume = dsutil::determinant3x3(lattice);
        Debug(format("Calculating equilibrium volume of cell from input file: {}", eqVolume), 1);
    }

    // Check if the paramters exist.
    double amount = stod(args.findArgument("AMOUNT")[0]) / 100;
    int totalSteps = stoi(args.findArgument("STEPS")[0]);

    // Get the list of target volumes.
    vector<double> targetVolumes;
    double stepSize = (amount * 0.5) / totalSteps;
    targetVolumes.push_back(eqVolume);

    for (int i = 1; i < (totalSteps * 0.5); ++i)
    {
        double _posVolume = eqVolume * (1 + (i * stepSize));
        double _negVolume = eqVolume * (1 - (i * stepSize));
        targetVolumes.push_back(_posVolume);
        targetVolumes.push_back(_negVolume);
    }

    // Sort the targetVolumes vector.
    std::sort(targetVolumes.begin(), targetVolumes.end());

    // For each target volume, scale the cell appropriatley.
    for (int j = 0; j < targetVolumes.size(); ++j)
    {
        Debug(format("Processing volume: {}", targetVolumes[j]), 1);

        // Get the scaling factor for each matrix element.
        double difference = targetVolumes[j] - eqVolume;
        double scalingFactor = 1.0 + ((1.0 / 3.0) * (difference / eqVolume));

        // Apply the scaling factor to each element of the matrix.
        array<array<double, 3>, 3> newMatrix = {};

        for (int a = 0; a < lattice.size(); ++a)
        {
            for (int b = 0; b < lattice[a].size(); ++b)
            {
                newMatrix[a][b] = (scalingFactor)*lattice[a][b];
            }
        }

        // Check the new determinant
        Debug(format("Actual volume: {}", determinant3x3(newMatrix)), 3);

        // Write the new file without updating the current working file.
        string path;
        if (args.hasArgument("OUTPUT_DIR"))
            path = format("{}/volume_{}/POSCAR", args.findArgument("OUTPUT_DIR")[0], j);
        else
            path = format("volume_{}/POSCAR", j);

        FileWriter writer;
        writer.writeToFile(path, "VASP", atoms, newMatrix);
    }
}
