#include "Atom.h"
#include "dsutil.h"

/// <summary>
/// Default constructor for atom.
/// </summary>
Atom::Atom()
{
    atomType = "H";
    atomPosition = Vector3D();
}

Atom::Atom(const string &type, const Vector3D &position)
{
    atomType = type;
    atomPosition = position;
}
