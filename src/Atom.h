#pragma once

#include "Atom.h"
#include "dsutil.h"
#include <string>

using namespace std;
using namespace dsutil;

class Atom
{
  public:
    Atom();
    Atom(const string &type, const Vector3D &position);
    Vector3D atomPosition;
    string atomType;
};
