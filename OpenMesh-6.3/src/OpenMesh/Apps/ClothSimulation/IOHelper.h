#ifndef LOAD_MODEL_H
#define LOAD_MODEL_H

// -------------------- OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Utils/getopt.h>

#include "GlobalTypes.h"

using namespace OpenMesh;

class IOHelper
{
public:
    int init(int argc, char **argv);

    int readin(int ofs, char** argv, MyMesh &mesh);

    int writeto(int ofs, char **argv, MyMesh &mesh);

  private:
    void parse_commandline(int _argc, char **_argv, MyMesh &_mesh, IO::Options &ropt, IO::Options &wopt);
    void usage_and_exit(int xcode);

    IO::Options m_ropt, m_wopt;
};

#endif // endif LOAD_MODEL_H