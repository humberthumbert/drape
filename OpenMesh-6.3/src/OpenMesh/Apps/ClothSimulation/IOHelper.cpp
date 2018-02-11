#include <iostream>
#include <iterator>
// -------------------- OpenMesh
#include "IOHelper.h"

// ----------------------------------------------------------------------------
using namespace OpenMesh;

// ----------------------------------------------------------------------------
#define CHKROPT(Option)                   \
    std::cout << "  provides " << #Option \
	      << (m_ropt.check(IO::Options::Option) ? ": yes\n" : ": no\n")
#define CHKWOPT(Option)                \
    std::cout << "  write " << #Option \
	      << (m_wopt.check(IO::Options::Option) ? ": yes\n" : ": no\n")
#define MESHOPT(msg, tf) \
    std::cout << "  " << msg << ": " << ((tf) ? "yes\n" : "no\n")

// ----------------------------------------------------------------------------
int IOHelper::init(int argc, char **argv)
{
    MyMesh mesh; // place holder
    parse_commandline(argc, argv, mesh, m_ropt, m_wopt);
    return 0;
}

int IOHelper::readin(int ofs, char **argv, MyMesh &mesh)
{
    if (!IO::read_mesh(mesh, argv[optind + ofs], m_ropt))
    {
        std::cerr << "Error loading mesh from file " << argv[optind] << std::endl;
        return 1;
    }
    return 0;
}

int IOHelper::writeto(int ofs, char **argv, MyMesh &mesh)
{
    // -------------------- write mesh
    std::cout << "Write mesh to " << argv[optind + ofs] << "..";
    if (!IO::write_mesh(mesh, argv[optind + ofs], m_wopt))
    {
		std::cerr << "Error" << std::endl;
		std::cerr << "Possible reasons:\n";
		std::cerr << "1. Chosen format cannot handle an option!\n";
		std::cerr << "2. Mesh does not provide necessary information!\n";
		std::cerr << "3. Or simply cannot open file for writing!\n";
		return 1;
    }
    else
	{
		std::cout << "Ok.\n";
	}

    return 0;
}

void IOHelper::parse_commandline(int _argc, char **_argv, MyMesh &_mesh,
				 IO::Options &ropt, IO::Options &wopt)
{
    int c;
    while ((c = getopt(_argc, _argv, "bhsBF:LMSV:X:")) != -1)
    {
	switch (c)
	{
	// -------------------- read options
	// force binary input
	case 'b':
	    ropt += IO::Options::Binary;
	    break;
	// force swapping the byte order, when reading a binary file
	case 's':
	    ropt += IO::Options::Swap;
	    break;
	// -------------------- write options
	// Write binary variant of format if possible
	case 'B':
	    wopt += IO::Options::Binary;
	    break;
	//
	case 'F':
	    for (size_t i = 0; optarg[i]; ++i)
		switch (optarg[i])
		{
		case 'n':
		    wopt += IO::Options::FaceNormal;
		    break;
		case 'c':
		    wopt += IO::Options::FaceColor;
		    break;
		}
	    break;
	// Use little endian when writing binary data
	case 'L':
	    wopt += IO::Options::LSB;
	    break;
	// Use big endian when writing binary data
	case 'M':
	    wopt += IO::Options::MSB;
	    break;
	// Swap byte order when writing binary data
	case 'S':
	    wopt += IO::Options::Swap;
	    break;
	//
	case 'V':
	{
	    for (size_t i = 0; optarg[i]; ++i)
		switch (optarg[i])
		{
		case 'n': // dont't change layout!!
		    wopt += IO::Options::VertexNormal;
		    break;
		case 't':
		    wopt += IO::Options::VertexTexCoord;
		    break;
		case 'c':
		    wopt += IO::Options::VertexColor;
		    break;
		}
	    break;
	}
	// -------------------- request mesh' standard properties
	// case 'X':
	// {
	//     char entity = '\0';
	//     for (size_t i = 0; optarg[i]; ++i)
	// 	switch (optarg[i])
	// 	{
	// 	case 'v':
	// 	case 'f':
	// 	    entity = optarg[i];
	// 	    break;
	// 	case 'n':
	// 	    switch (entity)
	// 	    {
	// 	    case 'v':
	// 		_mesh.request_vertex_normals();
	// 		break;
	// 	    case 'f':
	// 		_mesh.request_face_normals();
	// 		break;
	// 	    }
	// 	    break;
	// 	case 'c':
	// 	    switch (entity)
	// 	    {
	// 	    case 'v':
	// 		_mesh.request_vertex_colors();
	// 		break;
	// 	    case 'f':
	// 		_mesh.request_face_colors();
	// 		break;
	// 	    }
	// 	    break;
	// 	case 't':
	// 	    switch (entity)
	// 	    {
	// 	    case 'v':
	// 		_mesh.request_vertex_texcoords2D();
	// 		break;
	// 	    }
	// 	    break;
	// 	}
	//     break;
	// }

	// -------------------- help
	case 'h':
	    usage_and_exit(0);
	default:
	    usage_and_exit(1);
	}
    }
    // if (_argc - optind != 2)
	// usage_and_exit(1);
}
// ----------------------------------------------------------------------------
void IOHelper::usage_and_exit(int xcode)
{
    std::ostream &os = xcode ? std::cerr : std::cout;
    os << "Usage: io_options [Options] <input1> <input2> <output>\n"
       << std::endl;
    os << "  Read and write a mesh, using OpenMesh::IO::Options\n"
       << std::endl;
    os << "Options:\n"
       << std::endl;
    os << "a) read options\n"
       << std::endl
       << "  -b\n"
       << "\tAssume input file is a binary file\n"
       << std::endl
       << "  -s\n"
       << "\tSwap byte order when reading a binary file!\n"
       << std::endl;
    os << "b) write options\n"
       << std::endl
       << "  -B\n"
       << "\tWrite binary data\n"
       << std::endl
       << "  -S\n"
       << "\tSwap byte order, when writing binary data\n"
       << std::endl
       << "  -M/-L\n"
       << "\tUse MSB/LSB byte ordering, when writing binary data\n"
       << std::endl
       << "  -V{n|t|c}\n"
       << "\tWrite vertex normals, texcoords, and/or colors\n"
       << std::endl
       << "  -F{n|c}\n"
       << "\tWrite face normals, and/or colors\n"
       << std::endl;
    // os << "c) Mesh properties\n"
    //    << std::endl
    //    << "  -Xv{n|c|t}\n"
    //    << "\tRequest vertex property normals|colors|texcoords\n"
    //    << std::endl
    //    << "  -Xf{n|c}\n"
    //    << "\tRequest face property normals|colors\n"
    //    << std::endl;
    exit(xcode);
}
// end of file
// ============================================================================
