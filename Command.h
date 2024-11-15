#pragma once
#include "Cluster.h"
#include "CommandArguments.h"
#include "CrystalStructure.h"
#include "FileWriter.h"
#include "dsutil.h"
#include "global.h"
#include <algorithm>
#include <array>
#include <iterator>
#include <random>
#include <sstream>
#include <string>

class Command
{
  protected:
    virtual void checkInputFile(CommandArguments &args);
    virtual void checkOutputDirectory(CommandArguments &args);
    virtual bool hasArguments(CommandArguments &args, const vector<string> &required, const string &errorMessage);

  public:
    virtual ~Command() = default;
    virtual void execute(CommandArguments &args) = 0;
};

class Vacancy : public Command
{
  public:
    void execute(CommandArguments &args) override;

  private:
    // Parameters
    string bravais;
    int dim;
    vector<Cluster> clusters;
    bool printAll;
    bool deleteAtoms;

    // Methods
    void generateClusters(const map<int, Atom> &atoms, const int &size, const int &maxNN);
    double calculateOrderParameter(const Cluster &cluster);
    double calculateDistance(int atomIndex1, int atomIndex2);
    void logClusters();
    void writeVacancies(CommandArguments &args);
};

class Copy : public Command
{
  public:
    void execute(CommandArguments &args) override;
};

class Substitute : public Command
{
  public:
    void execute(CommandArguments &args) override;
};

class Recenter : public Command
{
  public:
    void execute(CommandArguments &args) override;
};

class Volume : public Command
{
  public:
    void execute(CommandArguments &args) override;
};

class SetGlobals : public Command
{
  public:
    void execute(CommandArguments &args) override;
};

class TetrahedralInterstitial : public Command
{
  public:
    void execute(CommandArguments &args) override;

  private:
    int targetAtomIndex;
    vector<Vector3D> sites;

    void getTargetAtomIndex(CommandArguments &args);
    void generateSites(CommandArguments &args);
    void populateSites(CommandArguments& args);
};
