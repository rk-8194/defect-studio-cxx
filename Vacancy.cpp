#include "Cluster.h"
#include "Command.h"
#include <format>
#include <set>

using namespace dsutil;

void Vacancy::execute(CommandArguments &args)
{
    // Break cases.
    if (!args.hasArgument("ELEMENT") || !args.hasArgument("CLUSTER") || !args.hasArgument("BRAVAIS") ||
        !args.hasArgument("DIM"))
    {
        Debug("Must specify the values of arguments ELEMENT, DIM, BRAVAIS and CLUSTER when performing vacancy setup.",
              -1);
        return;
    }

    // Check for a new input file. Set the work structure if a new input file is loaded.
    checkInputFile(args);

    // Check for an output directory. This function will make the required directories if necessary.
    checkOutputDirectory(args);

    // Get the list of atoms in the current working structure.
    map<int, Atom> atoms = g_workStructure.getAtoms();

    string element = args.findArgument("ELEMENT")[0];
    string amountStr = args.findArgument("CLUSTER")[0];
    dim = stoi(args.findArgument("DIM")[0]);
    bravais = args.findArgument("BRAVAIS")[0];
    printAll = args.hasArgument("ALL");

    // Parse the CLUSTER argument, which could be either a number or a "XvY" format.
    int amount, maxNN;

    if (amountStr.find("v") != string::npos) // Format XvY (e.g., "2v3")
    {
        size_t pos = amountStr.find("v");
        try
        {
            amount = stoi(amountStr.substr(0, pos)); // Parse the first number
            maxNN = stoi(amountStr.substr(pos + 1)); // Parse the second number
        }
        catch (const invalid_argument &e)
        {
            Debug("Invalid amount format in CLUSTER argument. Expected 'XvY'.", -1);
            return;
        }
    }
    else
    {
        Debug("Invalid format in CLUSTER argument. Expected 'XvY'.", -1);
        return;
    }

    // Start generating clusters.
    generateClusters(atoms, amount, maxNN);

    logClusters();
    writeVacancies(args);
}

void Vacancy::generateClusters(const map<int, Atom> &atoms, const int &size, const int &maxNN)
{
    for (int n = 1; n <= size; ++n)
    {
        Debug(format("Generating clusters of size: {}", n), 1);

        vector<Cluster> _newClusters;

        vector<double> _orderParameters;

        // If no clusters have been added, add the first vacancy.
        if (clusters.empty())
        {
            Cluster newCluster = Cluster();
            newCluster.clusterName = "1v1";
            newCluster.addAtom(1, 0); // Assuming you want to add the atom with index 1
            _newClusters.push_back(newCluster);
        }
        else
        {
            // Iterate through all existing clusters
            for (int i = 0; i < clusters.size(); ++i)
            {
                // Only consider clusters of size n-1
                if (clusters[i].getClusterSize() != (n - 1))
                    continue;

                // Loop over all atoms in the cluster.
                for (int j = 0; j < clusters[i].atomIndecies.size(); ++j)
                {
                    // Get the index of an atom and its neighbors
                    int _index = clusters[i].atomIndecies[j].first;
                    map<double, vector<int>> neighbours = clusters[i].getNeighbours(j, atoms);

                    // Make a new cluster out of each neighbor, but only process the first `maxNN` unique distances
                    int processedDistances = 0;
                    set<double> processedDistancesSet; // This set will store distances we have already processed

                    for (const auto &[distance, atomIndices] : neighbours)
                    {
                        if (processedDistances >= maxNN)
                            break; // Stop after processing maxNN unique distances

                        // Check if this distance has already been processed
                        if (processedDistancesSet.find(distance) != processedDistancesSet.end())
                            continue; // Skip if the distance has already been processed

                        // Add the distance to the set to mark it as processed
                        processedDistancesSet.insert(distance);

                        // Iterate through the vector of atom indices for this distance
                        for (int atomIndex : atomIndices)
                        {
                            // Create a new cluster with the current cluster and add the neighboring atom
                            Cluster newCluster = Cluster(clusters[i]);
                            newCluster.addAtom(atomIndex + 1, distance); // Assuming `addAtom` adds the atom by index

                            double _orderParameter = calculateOrderParameter(newCluster);

                            if (find(_orderParameters.begin(), _orderParameters.end(), _orderParameter) ==
                                _orderParameters.end() || printAll)
                            {
                                newCluster.orderParamter = _orderParameter;
                                newCluster.clusterName = format("{}v{}", n, processedDistances + 1);
                                _newClusters.push_back(newCluster); // Add the new cluster to _newClusters
                                _orderParameters.push_back(_orderParameter);
                            }
                            else
                            {
                                Debug(format("Cluster {} is degenerate - Order parameter: {}", i, _orderParameter), 3);
                            }
                        }

                        ++processedDistances; // Increment the counter after processing each unique distance
                    }
                }
            }
        }

        // Add the new clusters to the list of clusters.
        for (const auto &newCluster : _newClusters)
        {
            clusters.push_back(newCluster);
        }
    }
}

double Vacancy::calculateOrderParameter(const Cluster &cluster)
{
    int clusterSize = cluster.atomIndecies.size();

    double d_0 = sqrt(3.0 / 4.0) / dim; // Ideal separation for BCC structure in fractional units

    double sum = 0;

    // Iterate over all pairs of atoms in the cluster to calculate pairwise distances
    for (int j = 0; j < cluster.atomIndecies.size(); ++j)
    {
        for (int k = j + 1; k < cluster.atomIndecies.size(); ++k)
        {
            int atomIndexJ = cluster.atomIndecies[j].first;
            int atomIndexK = cluster.atomIndecies[k].first;

            // Calculate the distance between atoms j and k using the updated method (in fractional coordinates)
            double distance = calculateDistance(atomIndexJ, atomIndexK);

            // Debugging the calculated distance in fractional coordinates
            dsutil::Debug(
                format("Distance between atom {} and atom {} (in fractional): {}", atomIndexJ, atomIndexK, distance),
                3);

            // Normalize the squared distance by the ideal separation d_0 in fractional units
            sum += (distance * distance) / (d_0 * d_0);
        }
    }

    // Normalize the sum by the number of unique pairs (N * (N - 1) / 2)
    double orderParameter = sum / (clusterSize * (clusterSize - 1) / 2);

    double scale = std::pow(10.0, 5);
    orderParameter = round(orderParameter * scale) / scale;

    // Debugging the ordering parameter
    Debug(format("Cluster ordering parameter: {}", orderParameter), 1);

    return orderParameter;
}

double Vacancy::calculateDistance(int atomIndex1, int atomIndex2)
{
    // Fetch the atomic positions for atomIndex1 and atomIndex2 (in fractional coordinates)
    Atom atom1 = g_workStructure.getAtoms()[atomIndex1];
    Atom atom2 = g_workStructure.getAtoms()[atomIndex2];

    // Calculate the fractional difference
    double dx = atom1.atomPosition.x - atom2.atomPosition.x;
    double dy = atom1.atomPosition.y - atom2.atomPosition.y;
    double dz = atom1.atomPosition.z - atom2.atomPosition.z;

    // Apply periodic boundary conditions (minimum image convention) for fractional coordinates
    dx -= round(dx); // Wrap in x-dimension
    dy -= round(dy); // Wrap in y-dimension
    dz -= round(dz); // Wrap in z-dimension

    // The distance in fractional space can be interpreted as the direct fractional distance
    double distance = sqrt(dx * dx + dy * dy + dz * dz);

    return distance;
}

/// <summary>
/// Removes the clustered atoms from each cluster in clusters.
/// </summary>
void Vacancy::logClusters()
{
    // Open a text file to write the cluster data
    std::ofstream outFile("clusters.txt");

    // Check if the file is open
    if (!outFile.is_open())
    {
        std::cerr << "Error: Unable to open file for writing!" << std::endl;
        return;
    }

    // Check if clusters are populated
    if (clusters.empty())
    {
        std::cerr << "Error: No clusters found!" << std::endl;
        return;
    }

    // Iterate over the clusters
    for (size_t i = 0; i < clusters.size(); ++i)
    {
        outFile << "Cluster " << i << ":\tchi: " << clusters[i].orderParamter << "\n";
        // Debugging: Check how many atoms are in the current cluster
        std::cout << "Cluster " << i << " has " << clusters[i].atomIndecies.size() << " atoms.\n";

        // Write the atom indices and associated values (from the pair) for the current cluster
        outFile << "  Atom Indices and Distances:\n";
        for (size_t j = 0; j < clusters[i].atomIndecies.size(); ++j)
        {
            // Debugging: Print each atom index and distance
            std::cout << "  Atom Index: " << clusters[i].atomIndecies[j].first
                      << ", Distance: " << clusters[i].atomIndecies[j].second << "\n";
            outFile << "    Atom Index: " << clusters[i].atomIndecies[j].first
                    << ", Distance: " << clusters[i].atomIndecies[j].second << "\n";
        }

        outFile << "\n"; // Separate each cluster with a newline for readability
    }

    std::cout << "Text file 'clusters.txt' created successfully!" << std::endl;
}

void Vacancy::writeVacancies(CommandArguments &args)
{
    // Check if clusters are populated
    if (clusters.empty())
    {
        std::cerr << "Error: No clusters found!" << std::endl;
        return;
    }

    // Get the total number of atoms in the current structure
    map<int, Atom> atoms = g_workStructure.getAtoms();
    std::cout << "Number of atoms in g_workStructure: " << atoms.size() << std::endl;

    // Iterate over the clusters
    for (size_t i = 0; i < clusters.size(); ++i)
    {
        // Create a new map to hold atoms that are not in the current cluster
        std::map<int, Atom> newAtoms;

        // Get the current cluster's atom indices
        const auto &clusterAtomIndices = clusters[i].atomIndecies;

        // Go over the old list of atoms in the work structure and add them to the new list
        // only if they are not in the current cluster
        for (int j = 0; j < atoms.size(); ++j)
        {
            // Check if the atom index is valid
            if (j >= atoms.size())
            {
                std::cerr << "Error: Invalid atom index " << j + 1 << " (out of bounds)" << std::endl;
                continue;
            }

            bool isInCluster = false;

            // Check if the atom is in the current cluster
            Atom atom = atoms[j + 1];
            for (const auto &atomIndexPair : clusterAtomIndices)
            {
                if (atomIndexPair.first == j + 1)
                {
                    isInCluster = true;
                    break;
                }
            }

            // If the atom is in the cluster, set its atom type to X.
            // This will enable it to be easily removed, or used to add further defects near the vacancy.
            if (isInCluster)
                atom.atomType = "X";

            newAtoms[j] = atom; // Use atom index as key and the atom object as value
        }

        // Make a new crystal structure with the newAtoms
        CrystalStructure newStructure = CrystalStructure(g_workStructure.getLattice(), newAtoms);

        // Write the new file without updating the current working file.
        std::string path;
        if (args.hasArgument("OUTPUT_DIR"))
        {
            path = format("{}/{}_{}/POSCAR", args.findArgument("OUTPUT_DIR")[0], clusters[i].clusterName, i);
        }
        else
        {
            path = format("{}_{}/POSCAR", clusters[i].clusterName, i);
        }

        // Use FileWriter to save the new structure
        FileWriter writer;
        writer.writeToFile(path, "VASP", newStructure);
    }
}
