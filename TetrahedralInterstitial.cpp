#include "Command.h"

/// <summary>
/// Creates tetrahedral interstitials around a given site.
/// CURRENTLY ONLY WORKS FOR BCC CRYSTAL STRUCTURES.
/// </summary>
/// <param name="args">- The arguments passed to the command.</param>
void TetrahedralInterstitial::execute(CommandArguments &args)
{
    // Check that the required arguments have been satisfied.
    if (!hasArguments(args, {"ELEMENT", "DIM"},
                      "Must specify a defect ELEMENT and the DIMensionality of the supercell."))
        return;

    // Check for a new input file. Set the work structure if a new input file is loaded.
    checkInputFile(args);

    // Check for an output directory. This function will make the required directories if necessary.
    checkOutputDirectory(args);

    // Get the list of atoms in the current working structure.
    map<int, Atom> atoms = g_workStructure.getAtoms();

    // Get the index of the target atom.
    getTargetAtomIndex(args);

    // Generate a list of positions for interstitial sites to be placed at.
    generateSites(args);

    // Place an interstitial defect at each site.
    populateSites(args);
}

/// <summary>
/// Gets the index of the target atom which the interstitial will be created around.
/// </summary>
/// <param name="args">- The list of arguments passed to the command.</param>
void TetrahedralInterstitial::getTargetAtomIndex(CommandArguments &args)
{
    // Runs in one of two modes:
    // 1) Specific atom index selected, or
    // 2) The first atom of type X.

    targetAtomIndex = -1;
    if (args.hasArgument("SELECT"))
        targetAtomIndex = stoi(args.findArgument("SELECT")[0]);
    else if (args.hasArgument("FIRST"))
    {
        // Get the first element in the getAtomsOfType() map.
        string _targetAtomType = args.findArgument("FIRST")[0];
        targetAtomIndex = g_workStructure.getAtomsOfType(_targetAtomType).begin()->first;
    }
}

void TetrahedralInterstitial::generateSites(CommandArguments &args)
{
    // In bcc, the relative positions of tetrahedral sites are given by the equation
    //          r = (+/- 0.5 a, +/- 0.25 b, c)

    // The position of the target atom.
    Vector3D targetPosition = g_workStructure.getAtoms()[targetAtomIndex].atomPosition;
    cout << format("{} {} {}", targetPosition.x, targetPosition.y, targetPosition.z) << endl;

    // Lattice parameter and dimensionality
    int dim = stoi(args.findArgument("DIM")[0]);
    double cellSize = g_workStructure.getLattice()[0][0];
    double latticeParameter = cellSize / (dim);

    // A list of all possible translations in BBC lattice.
    vector<Vector3D> translations;
    for (int dx : {-1, 1})
    {
        for (int dy : {-1, 1})
        {
            for (int dz : {-1, 1}) // Include the z-direction for translations
            {
                translations.push_back(Vector3D(0.5 * static_cast<double>(dx), 0.25 * static_cast<double>(dx),
                                                0 * static_cast<double>(dx)));

                cout << format("{} {} {}", translations[translations.size() - 1].x,
                               translations[translations.size() - 1].y, translations[translations.size() - 1].z)
                     << endl;
            }
        }
    }

    // Conversion factor from Cartesian lattice parameter to Fractional lattice parameter.
    double conversionFactor = latticeParameter / cellSize;

    // Add the new sites.
    for (const Vector3D &translation : translations)
    {
        double _posX = targetPosition.x + (translation.x * conversionFactor);
        double _posY = targetPosition.y + (translation.y * conversionFactor);
        double _posZ = targetPosition.z + (translation.z * conversionFactor);

        Vector3D sitePosition = Vector3D(_posX, _posY, _posZ);

        sites.push_back(sitePosition);
    }
}

void TetrahedralInterstitial::populateSites(CommandArguments &args)
{
    // Get the defect type.
    string defectType = args.findArgument("ELEMENT")[0];

    // Make a crystal structure with a new atom at each site.
    int count = 1;
    for (const Vector3D &site : sites)
    {
        // Copy the crystal structure.
        CrystalStructure newStructure = CrystalStructure(g_workStructure.getLattice(), g_workStructure.getAtoms());

        // Add the new atom.
        Atom newAtom = Atom(defectType, site);
        newStructure.addAtom(newStructure.getAtoms().size() + 1, newAtom);

        // Write the new file without updating the current working file.
        std::string path;
        if (args.hasArgument("OUTPUT_DIR"))
        {
            path = format("{}/{}_tis_{}/POSCAR", args.findArgument("OUTPUT_DIR")[0], defectType, count);
        }
        else
        {
            path = format("{}_tis_{}/POSCAR", defectType, count);
        }

        // Use FileWriter to save the new structure
        FileWriter writer;
        writer.writeToFile(path, "VASP", newStructure);
        ++count;
    }
}
