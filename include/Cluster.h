#pragma once
#include "Atom.h"
#include <format>
#include <vector>

class Cluster
{
  public:
    Cluster();
    Cluster(const Cluster &parent, const int &newIndex, const int &newIndexNN);

    vector<pair<int, double>> atomIndecies;
    double orderParamter;
    string clusterName;

    void addAtom(const int &atomIndex, const double &distance);
    int getClusterSize();
    map<double, vector<int>> getNeighbours(const int &index, map<int, Atom> atoms);
};
