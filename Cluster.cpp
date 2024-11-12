#include "Cluster.h"
#include "global.h"

Cluster::Cluster()
{
}

Cluster::Cluster(const Cluster &parent, const int &newIndex, const int &distance)
{
    for (int i = 0; i < parent.atomIndecies.size(); ++i)
    {
        addAtom(parent.atomIndecies[i].first, (parent.atomIndecies[i].second));
    }

    addAtom(newIndex, distance);
}

void Cluster::addAtom(const int &atomIndex, const double &distance)
{
    atomIndecies.push_back(make_pair(atomIndex, distance));
    Debug(format("Added atom {} to cluster. Nearest neighbour in coordination sphere {}", atomIndex, distance), 3);
}

map<double, vector<int>> Cluster::getNeighbours(const int &index, map<int, Atom> atoms)
{
    double tolerance = 1e-4; // Tolerance level for considering distances equivalent
    map<double, vector<int>> neighbours;
    Atom atom = atoms[index + 1];

    // Define the unit cell dimensions (these are for reference and might not be used directly)
    double cellX = g_workStructure.getLattice()[0][0] / 4;
    double cellY = g_workStructure.getLattice()[1][1] / 4;
    double cellZ = g_workStructure.getLattice()[2][2] / 4;

    // Iterate over all atoms to calculate the distance
    for (int i = 0; i < g_workStructure.getAtoms().size(); ++i)
    {
        if (i == index)
            continue; // Skip the atom itself

        // Check if the atom is already in the atomIndices list (exclude it if it's there)
        bool isInAtomIndices = false;
        for (const auto &atomIndexPair : atomIndecies)
        {
            if (atomIndexPair.first == i + 1) // Atom index is i + 1 due to 1-based indexing
            {
                isInAtomIndices = true;
                break;
            }
        }

        if (isInAtomIndices)
            continue; // Skip this atom if it is already in atomIndices

        // Get the neighbour atom's fractional coordinates
        double dx = atom.atomPosition.x - atoms[i + 1].atomPosition.x;
        double dy = atom.atomPosition.y - atoms[i + 1].atomPosition.y;
        double dz = atom.atomPosition.z - atoms[i + 1].atomPosition.z;

        // Apply periodic boundary conditions (minimum image convention) in fractional coordinates
        dx -= round(dx); // Wrap the x-dimension
        dy -= round(dy); // Wrap the y-dimension
        dz -= round(dz); // Wrap the z-dimension

        // Calculate the Euclidean distance in fractional space (no need to convert to Cartesian)
        double distance = sqrt(dx * dx + dy * dy + dz * dz);

        // Debugging the calculated distance
        Debug(format("Distance of {} to {}: {}", index + 1, i + 1, distance), 3);

        // Check if the distance is within the tolerance of an already existing one
        bool found = false;
        for (auto &entry : neighbours)
        {
            if (abs(entry.first - distance) < tolerance)
            {
                entry.second.push_back(i); // Add the neighbour atom index
                found = true;
                break;
            }
        }

        if (!found)
        {
            neighbours[distance].push_back(i); // Add the neighbour atom index
        }
    }

    return neighbours; // Return the neighbors with the calculated distances
}

int Cluster::getClusterSize()
{
    return atomIndecies.size();
}
