#include "Command.h"
#include <set>

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
    // If in RANDOM/PERCENT mode, it will only generate the first interstitial site for each selected atom.
    // If not, it will generate all interstitial sites for the single atom that was selected.
    if (args.hasArgument("RANDOM") && args.hasArgument("PERCENT"))
    {
        if (targetAtoms.empty())
        {
            Debug("No atoms selected for RANDOM and PERCENT operation.", -1);
            return; // Exit if no atoms were selected.
        }

        for (const auto &index : targetAtoms)
        {
            try
            {
                generateFirstSite(args, index);
                Debug(format("Successfully generated first site for atom index: {}", index), 3);
            }
            catch (const std::exception &e)
            {
                Debug(format("Failed to generate first site for atom index {}: {}", index, e.what()), -1);
            }
        }
    }
    else
    {
        generateSites(args, targetAtoms[0]);
    }

    // Place an interstitial defect at each site.
    populateSites(args);
}

/// <summary>
/// Gets the index of the target atom which the interstitial will be created around.
/// </summary>
/// <param name="args">- The list of arguments passed to the command.</param>
void TetrahedralInterstitial::getTargetAtomIndex(CommandArguments &args)
{
    // Runs in one of three modes:
    // 1) Specific atom index selected, or
    // 2) The first atom of type X.
    // 3) Random atoms of type X.

    if (args.hasArgument("SELECT"))
        targetAtoms.push_back(stoi(args.findArgument("SELECT")[0]));
    else if (args.hasArgument("FIRST"))
    {
        // Get the first element in the getAtomsOfType() map.
        string _targetAtomType = args.findArgument("FIRST")[0];
        targetAtoms.push_back(g_workStructure.getAtomsOfType(_targetAtomType).begin()->first);
    }
    else if (args.hasArgument("RANDOM") && args.hasArgument("PERCENT"))
    {
        // Extract the target atom type and percentage.
        string _targetAtomType = args.findArgument("RANDOM")[0];
        double _percent;
        try
        {
            _percent = stod(args.findArgument("PERCENT")[0]) / 100.0;
            if (_percent < 0.0 || _percent > 1.0)
                Debug("PERCENT must be between 0 and 100.", -1);
        }
        catch (const std::exception &e)
        {
            Debug("Invalid PERCENT argument: must be a number between 0 and 100.", -1);
        }

        // Get atoms of the specified type.
        auto atomsOfType = g_workStructure.getAtomsOfType(_targetAtomType);
        if (atomsOfType.empty())
        {
            Debug(format("No atoms of the specified type: {}", _targetAtomType), -1);
            return; // Exit as there are no atoms to process.
        }

        // Precompute atom indices.
        vector<int> atomIndices;
        for (const auto &entry : atomsOfType)
        {
            atomIndices.push_back(entry.first);
        }

        // Determine the number of atoms to select based on the percentage.
        int numberOfAtoms = floor(atomIndices.size() * _percent);
        if (numberOfAtoms == 0)
        {
            Debug("PERCENT value too small; no atoms selected.", -1);
            return;
        }
        else
        {
            Debug(format("Generating {} tetrahedral interstitial defects...", numberOfAtoms), 0);
        }

        // Setup random number generation.
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, atomIndices.size() - 1);

        // Randomly select the required number of unique atoms.
        for (int i = 0; i < numberOfAtoms; ++i)
        {
            bool atomSelected = false;
            while (!atomSelected)
            {
                // Pick a random atom index.
                int randomIndex = dis(gen);
                int selectedAtom = atomIndices[randomIndex];

                // Ensure the atom is not already in the target list.
                if (std::find(targetAtoms.begin(), targetAtoms.end(), selectedAtom) == targetAtoms.end())
                {
                    targetAtoms.push_back(selectedAtom);
                    Debug(format("Added {} to list of selected atoms.", selectedAtom), 3);
                    atomSelected = true; // Exit the loop.
                }
            }
        }
    }
}

void TetrahedralInterstitial::generateFirstSite(CommandArguments &args, const int &targetAtomIndex)
{
    // The position of the target atom.
    Vector3D targetPosition = g_workStructure.getAtoms()[targetAtomIndex].atomPosition;
    cout << format("{} {} {}", targetPosition.x, targetPosition.y, targetPosition.z) << endl;

    // Lattice parameter and dimensionality
    int dim = stoi(args.findArgument("DIM")[0]);
    double cellSize = g_workStructure.getLattice()[0][0];
    double latticeParameter = cellSize / (dim);

    // Generates the [0.5a, 0.25b, c] interstitial.
    Vector3D translation = Vector3D(0.5, 0.25, 0);

    // Conversion factor from Cartesian lattice parameter to Fractional lattice parameter.
    double conversionFactor = latticeParameter / cellSize;

    // Get the position.
    double _posX = targetPosition.x + (translation.x * conversionFactor);
    double _posY = targetPosition.y + (translation.y * conversionFactor);
    double _posZ = targetPosition.z + (translation.z * conversionFactor);

    Vector3D sitePosition = Vector3D(_posX, _posY, _posZ);

    sites.push_back(sitePosition);
}

void TetrahedralInterstitial::generateSites(CommandArguments &args, const int &targetAtomIndex)
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
    std::set<Vector3D> translations;
    // Generate the displacements from the central atom at (0, 0, 0)
    int a = 1;
    for (int dx : {-1, 1})
    { // Move along the x-axis
        for (int dy : {-1, 1})
        { // Move along the y-axis
            for (int dz : {-1, 1})
            { // Move along the z-axis
                // Generate all valid displacements, ensuring the values are unique
                translations.insert(Vector3D(0,             // x remains 0
                                             0.25 * dy * a, // Move by ±0.25a along the y-axis
                                             0.5 * dz * a   // Move by ±0.5a along the z-axis
                                             ));

                translations.insert(Vector3D(0,            // x remains 0
                                             0.5 * dy * a, // Move by ±0.5a along the y-axis
                                             0.25 * dz * a // Move by ±0.25a along the z-axis
                                             ));

                translations.insert(Vector3D(0.25 * dx * a, // Move by ±0.25a along the x-axis
                                             0,             // y remains 0
                                             0.5 * dz * a   // Move by ±0.5a along the z-axis
                                             ));

                translations.insert(Vector3D(0.5 * dx * a, // Move by ±0.5a along the x-axis
                                             0,            // y remains 0
                                             0.25 * dz * a // Move by ±0.25a along the z-axis
                                             ));

                translations.insert(Vector3D(0.25 * dx * a, // Move by ±0.25a along the x-axis
                                             0.5 * dy * a,  // Move by ±0.5a along the y-axis
                                             0              // z remains 0
                                             ));

                translations.insert(Vector3D(0.5 * dx * a,  // Move by ±0.5a along the x-axis
                                             0.25 * dy * a, // Move by ±0.25a along the y-axis
                                             0              // z remains 0
                                             ));
            }
        }
    }

    // Output the unique translations (interstitial sites)
    for (const auto &translation : translations)
    {
        std::cout << std::format("{} {} {}\n", translation.x, translation.y, translation.z);
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
    if (args.hasArgument("RANDOM") && args.hasArgument("PERCENT"))
    {
        // Copy the crystal structure.
        CrystalStructure newStructure = CrystalStructure(g_workStructure.getLattice(), g_workStructure.getAtoms());

        for (const Vector3D &site : sites)
        {

            // Add the new atom.
            Atom newAtom = Atom(defectType, site);
            newStructure.addAtom(newStructure.getAtoms().size() + 1, newAtom);
        }

        // Write the new file without updating the current working file.
        std::string path;
        if (args.hasArgument("OUTPUT_DIR"))
        {
            path = format("{}/{}_tis_{}/POSCAR", args.findArgument("OUTPUT_DIR")[0], defectType, sites.size());
        }
        else
        {
            path = format("{}_tis_{}/POSCAR", defectType, sites.size());
        }

        // Use FileWriter to save the new structure
        FileWriter writer;
        writer.writeToFile(path, "VASP", newStructure);
    }
    else
    {
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
}
