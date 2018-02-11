/* ========================================================================= *
 *                                                                           *
 *                               OpenMesh                                    *
 *           Copyright (c) 2001-2015, RWTH-Aachen University                 *
 *           Department of Computer Graphics and Multimedia                  *
 *                          All rights reserved.                             *
 *                            www.openmesh.org                               *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * This file is part of OpenMesh.                                            *
 *---------------------------------------------------------------------------*
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 *                                                                           *
 * 1. Redistributions of source code must retain the above copyright notice, *
 *    this list of conditions and the following disclaimer.                  *
 *                                                                           *
 * 2. Redistributions in binary form must reproduce the above copyright      *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. Neither the name of the copyright holder nor the names of its          *
 *    contributors may be used to endorse or promote products derived from   *
 *    this software without specific prior written permission.               *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED *
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              *
 *                                                                           *
 * ========================================================================= */
/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/
#include <iostream>
// -------------------- OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

// -------------------- OpenGL
#if defined(WIN32) || defined(linux)
  #include <GL/gl.h>
  #include <GL/glut.h>
#elif defined(__APPLE__)
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#endif

// ----------------------------------------------------------------------------
typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;
// ----------------------------------------------------------------------------
// Build a simple cube and write it to std::cout
	
MyMesh mesh;
	
void init()
{
    glShadeModel(GL_SMOOTH);
    glClearColor(.2f, .2f, .4f, .5f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightpos[] = {10.0, 10.0, 10.0, 0.};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    GLfloat white[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat cyan[] = {0.f, .8f, .8f, 1.f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    GLfloat shininess[] = {50};
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    // myMesh.request_vertex_normals();
    // myMesh.request_face_normals();
    // IO::Options myOption = optind;
    // myOption+= IO::Options::VertexNormal;
    // myOption+= IO::Options::FaceNormal;
    // std::cout<<"VN? "<< myOption.check(IO::Options::VertexNormal) <<std::endl;
    // // myOption+= IO::Options::VertexTexCoord;
    // bool success = IO::read_mesh( myMesh, "arm_16k.ply", myOption);
    // myMesh.update_normals();
    // std::cout<<success<<std::endl;

    // //Initializtin collision check;
    // mBody.init(myMesh);
}

void display()
{
    std::cout<<"Before Display"<<std::endl;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);  
    glLoadIdentity(); 
    gluLookAt(2, 0, 0, 0, 0, 0, 0, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glColor3f(0.5, 0.5, 0.0);
    // Print all the faces
	for(MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    
		for (MyMesh::FaceVertexIter fv_it=mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
  		{
	    	// do something with e.g. mesh.point(*vv_it)
			//std::cout << "The Face #" << *f_it << "'s valence is " << mesh.point(*fv_it) << std::endl;
			glVertex3f(mesh.point(*fv_it)[0], mesh.point(*fv_it)[1], mesh.point(*fv_it)[2]);    	
		}
    }
	
    /*MyMesh::VertexIter v_it, v_end(mesh.vertices_end());
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it){
    	glVertex3f(mesh.point(*v_it)[0], mesh.point(*v_it)[1], mesh.point(*v_it)[2]);
    	++v_it;
    	bool end = false;
    	if(v_it==v_end){
    		v_it = mesh.vertices_begin();
    		end = true;
    	}
    	glVertex3f(mesh.point(*v_it)[0], mesh.point(*v_it)[1], mesh.point(*v_it)[2]);
    	if(end){
    		break;
    	}
    	--v_it;
    	std::cerr << mesh.point(*v_it) << std::endl;
    
	}
	*/
/*    for (unsigned int x = 0; x < faces.size(); ++x)
    {
        glVertex3f(faces[x].points[0]->getPos().m_x, faces[x].points[0]->getPos().m_y, faces[x].points[0]->getPos().m_z);
        glVertex3f(faces[x].points[1]->getPos().m_x, faces[x].points[1]->getPos().m_y, faces[x].points[1]->getPos().m_z);
        glVertex3f(faces[x].points[2]->getPos().m_x, faces[x].points[2]->getPos().m_y, faces[x].points[2]->getPos().m_z);
  

    }*/
    glEnd(); 



    glutSwapBuffers();
    std::cout<<"After Display"<<std::endl;

}

void keyboardFunc(unsigned char key, int x, int y)
{
    switch (key) {
        case '\033': 
            exit(0);
            break;
        default:
            break;
    }
}

void idleFunc()
{
    glutPostRedisplay();
}

void reshape(int w, int h)  
{
    std::cout<<"Before Reshape"<<std::endl;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();  
    if (h==0)  
        gluPerspective (80, (float)w,1.0,5000.0);
    else
        gluPerspective (80, (float)w /( float )h,1.0,5000.0 );
    std::cout<<"After Reshape"<<std::endl;

}

int main(int argc, char* argv[])
{
	if (argc!=2)
	{
		std::cerr << "Usage: " << argv[0] << " <input>\n";
		return 1;
	}
	// request vertex normals, so the mesh reader can use normal information
	// if available
	mesh.request_vertex_normals();
	// assure we have vertex normals
	if (!mesh.has_vertex_normals())
	{
		std::cerr << "ERROR: Standard vertex property 'Normals' not available!\n";
		return 1;
	}
	OpenMesh::IO::Options opt;
	if ( ! OpenMesh::IO::read_mesh(mesh,argv[1], opt))
	{
    	std::cerr << "Error loading mesh from file " << argv[1] << std::endl;
		return 1;
	}
	// If the file did not provide vertex normals, then calculate them
	if ( !opt.check( OpenMesh::IO::Options::VertexNormal ) )
	{
    	// we need face normals to update the vertex normals
    	mesh.request_face_normals();
    	// let the mesh update the normals
    	mesh.update_normals();
    	// dispose the face normals, as we don't need them anymore
    	mesh.release_face_normals();
	}
	// Print all the vertice
	for (MyMesh::VertexIter v_it = mesh.vertices_begin();
		v_it != mesh.vertices_end(); ++v_it)
	{
	std::cout << "Vertex #" << *v_it << ": " << mesh.point( *v_it ) << std::endl;
	}
	
	// don't need the normals anymore? Remove them!
	mesh.release_vertex_normals();
	// just check if it really works
	if (mesh.has_vertex_normals())
	{
	  std::cerr << "Ouch! ERROR! Shouldn't have any vertex normals anymore!\n";
	 	return 1;
	}
	glutInit(&argc,argv);
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH ); 
	glutInitWindowSize( 1280, 720 ); 
	glutCreateWindow( "Cube Sample" );
	init();
	glutDisplayFunc(display);  
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboardFunc);
	glutIdleFunc(idleFunc);
	glutMainLoop();
	return 0;
}

/*
int main(int argc, char* argv[])
{
	// generate vertices
	MyMesh::VertexHandle vhandle[8];
	vhandle[0] = mesh.add_vertex(MyMesh::Point(-100, -100,  100));
	vhandle[1] = mesh.add_vertex(MyMesh::Point( 100, -100,  100));
	vhandle[2] = mesh.add_vertex(MyMesh::Point( 100,  100,  100));
	vhandle[3] = mesh.add_vertex(MyMesh::Point(-100,  100,  100));
	vhandle[4] = mesh.add_vertex(MyMesh::Point(-100, -100, -100));
	vhandle[5] = mesh.add_vertex(MyMesh::Point( 100, -100, -100));
	vhandle[6] = mesh.add_vertex(MyMesh::Point( 100,  100, -100));
	vhandle[7] = mesh.add_vertex(MyMesh::Point(-100,  100, -100));

	std::vector<MyMesh::VertexHandle>  face_vhandles;
	// generate (quadrilateral) faces
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[3]);
	mesh.add_face(face_vhandles);
 
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[4]);
	mesh.add_face(face_vhandles);
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[5]);
	mesh.add_face(face_vhandles);
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[6]);
	mesh.add_face(face_vhandles);
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[7]);
	mesh.add_face(face_vhandles);
	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[4]);
	mesh.add_face(face_vhandles);
	// write mesh to output.obj
	try
	{
		if ( !OpenMesh::IO::write_mesh(mesh, "output.off") )
		{
			std::cerr << "Cannot write mesh to file 'output.off'" << std::endl;
			return 1;
		} else {
			glutInit(&argc,argv);
			glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH ); 
			glutInitWindowSize( 1280, 720 ); 
			glutCreateWindow( "Cube Sample" );
			init();

			glutDisplayFunc(display);  
			glutReshapeFunc(reshape);
			glutKeyboardFunc(keyboardFunc);
			glutIdleFunc(idleFunc);
			glutMainLoop();

		}
	}
	catch( std::exception& x )
	{
	std::cerr << x.what() << std::endl;
	return 1;
	}
	return 0;
}*/