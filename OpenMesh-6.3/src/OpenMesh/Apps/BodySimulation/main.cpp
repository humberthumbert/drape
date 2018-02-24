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

#include "Body.h"

// ----------------------------------------------------------------------------
typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;
// ----------------------------------------------------------------------------
// Build a simple cube and write it to std::cout
	
MyMesh mesh;
Body* body;
// mouse control state
typedef enum { ROTATE, TRANSLATE, SCALE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;
typedef enum { X_AXIS, Y_AXIS, Z_AXIS} ROTATE_AXIS;

int leftMouseButton = 0; // 1 if pressed, 0 if not 
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not
// mouse coordinate
int mousePos[2]; 
// state of the world
float landRotate[3] = { 0.0f, 0.0f, 0.0f };
float landTranslate[3] = { 0.0f, 0.0f, 0.0f };
float landScale[3] = { 1.0f, 1.0f, 1.0f };

void init()
{
    glShadeModel(GL_SMOOTH);
    //glClearColor(.2f, .2f, .4f, .5f);
    glEnable(GL_DEPTH_TEST);
    /*glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightpos[] = {2.0, 2.0, 2.0, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    GLfloat white[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat cyan[] = {0.f, .8f, .8f, 1.f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    GLfloat shininess[] = {50};
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
*/

}
void rotate(float& x, float& y, float& z){
	// rotate x
	float sin_t = sin(landRotate[0]);
   	float cos_t = cos(landRotate[0]);
   	y = y * cos_t - z * sin_t;
    z = z * cos_t + y * sin_t;
    // rotate y
    sin_t = sin(landRotate[1]);
   	cos_t = cos(landRotate[1]);
   	x = x * cos_t - z * sin_t;
    z = z * cos_t + x * sin_t;
    // rotate z
    sin_t = sin(landRotate[2]);
   	cos_t = cos(landRotate[2]);
   	x = x * cos_t - y * sin_t;
    y = y * cos_t + x * sin_t;    
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);  
    glLoadIdentity(); 
    gluLookAt(0, 0, 4, 0, 0, 0, 0, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    // Print all the faces
    glBegin(GL_TRIANGLES);

    glColor3f(0.0, 1.0, 0.0);
    mesh = body->getBody();
	for(MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
		
		for (MyMesh::FaceVertexIter fv_it=mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
  		{
	    	// do something with e.g. mesh.point(*fv_it)
	    	float x = mesh.point(*fv_it)[0]; float y = mesh.point(*fv_it)[1]; float z = mesh.point(*fv_it)[2];
	    	rotate(x, y, z);
			//glVertex3f(mesh.point(*fv_it)[0], mesh.point(*fv_it)[1], mesh.point(*fv_it)[2]);
			glVertex3f(x, y, z);    	
		}
    }
    glEnd();
	

    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    std::vector<MyMesh::Point> landmarks = body->getLandmarks();
    for(unsigned i = 0; i < landmarks.size();i++){
	    float x = landmarks[i][0], y = landmarks[i][1], z = landmarks[i][2];
	    rotate(x, y, z);
	    glVertex3f(x, y, z);
	   	x = 0; y = -10; z = 0;
	   	rotate(x, y, z);
	    glVertex3f(x, y, z);
	}/*
	for(unsigned i = 0; i < body->getXSegment(20).size(); i++){
	    float x = body->getXSegment(20)[i][0], y = body->getXSegment(20)[i][1], z = body->getXSegment(20)[i][2];
	    rotate(x, y, z);
	    glVertex3f(x, y, z);
	   	x = 0; y = -10; z = 0;
	   	rotate(x, y, z);
	    glVertex3f(x, y, z);	
	}

	for(unsigned i = 0; i < body->getXSegment(80).size(); i++){
	    float x = body->getXSegment(80)[i][0], y = body->getXSegment(80)[i][1], z = body->getXSegment(80)[i][2];
	    rotate(x, y, z);
	    glVertex3f(x, y, z);
	   	x = 0; y = -10; z = 0;
	   	rotate(x, y, z);
	    glVertex3f(x, y, z);	
	}
	for(unsigned i = 0; i < body->getYSegment(65).size(); i++){
	    float x = body->getYSegment(65)[i][0], y = body->getYSegment(65)[i][1], z = body->getYSegment(65)[i][2];
	    rotate(x, y, z);
	    glVertex3f(x, y, z);
	   	x = 0; y = 0; z = -10;
	   	rotate(x, y, z);
	    glVertex3f(x, y, z);	
	}
	for(unsigned i = 0; i < body->getYSegment(80).size(); i++){
	    float x = body->getYSegment(80)[i][0], y = body->getYSegment(80)[i][1], z = body->getYSegment(80)[i][2];
	    rotate(x, y, z);
	    glVertex3f(x, y, z);
	   	x = 0; y = 0; z = -10;
	   	rotate(x, y, z);
	    glVertex3f(x, y, z);	
	}*/
    glEnd(); 



    glutSwapBuffers();
    
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


void mouseMotionDragFunc(int x, int y)
{
	// mouse has moved and one of the mouse buttons is pressed (dragging)

	// the change in mouse position since the last invocation of this function
	int mousePosDelta[2] = { x - mousePos[0], y - mousePos[1] };

	switch (controlState)
	{
		// translate the landscape
	case TRANSLATE:
		if (leftMouseButton)
		{
			// control x,y translation via the left mouse button
			landTranslate[0] += mousePosDelta[0] * 0.1f;
			landTranslate[1] -= mousePosDelta[1] * 0.1f;
		}
		if (middleMouseButton)
		{
			// control z translation via the middle mouse button
			landTranslate[2] += mousePosDelta[1] * 0.1f;
		}
		break;

		// rotate the landscape
	case ROTATE:
		if (leftMouseButton)
		{
			// control x,y rotation via the left mouse button
			landRotate[0] += mousePosDelta[1] * 0.01f;
			landRotate[1] += mousePosDelta[0] * 0.01f;
		}
		if (middleMouseButton)
		{
			// control z rotation via the middle mouse button
			landRotate[2] += mousePosDelta[1] * 0.01f;
		}
		break;

		// scale the landscape
	case SCALE:
		if (leftMouseButton)
		{
			// control x,y scaling via the left mouse button
			landScale[0] *= 1.0f + mousePosDelta[0] * 0.01f;
			landScale[1] *= 1.0f - mousePosDelta[1] * 0.01f;
		}
		if (middleMouseButton)
		{
			// control z scaling via the middle mouse button
			landScale[2] *= 1.0f - mousePosDelta[1] * 0.01f;
		}
		break;
	}

	// store the new mouse position
	mousePos[0] = x;
	mousePos[1] = y;
}

void mouseMotionFunc(int x, int y)
{
	// mouse has moved
	// store the new mouse position
	mousePos[0] = x;
	mousePos[1] = y;
}

void mouseButtonFunc(int button, int state, int x, int y)
{
	// a mouse button has has been pressed or depressed

	// keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		leftMouseButton = (state == GLUT_DOWN);
		break;

	case GLUT_MIDDLE_BUTTON:
		middleMouseButton = (state == GLUT_DOWN);
		break;

	case GLUT_RIGHT_BUTTON:
		rightMouseButton = (state == GLUT_DOWN);
		break;
	}

	// keep track of whether CTRL and SHIFT keys are pressed
	switch (glutGetModifiers())
	{
	case GLUT_ACTIVE_CTRL:
		controlState = TRANSLATE;
		break;

	case GLUT_ACTIVE_SHIFT:
		controlState = SCALE;
		break;

		// if CTRL and SHIFT are not pressed, we are in rotate mode
	default:
		controlState = ROTATE;
		break;
	}

	// store the new mouse position
	mousePos[0] = x;
	mousePos[1] = y;
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
	
	// Body
	body = new Body(mesh, 183);

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
	glutIdleFunc(idleFunc);
	// callback for mouse drags
	glutMotionFunc(mouseMotionDragFunc);
	// callback for idle mouse movement
	glutPassiveMotionFunc(mouseMotionFunc);
	// callback for mouse button changes
	glutMouseFunc(mouseButtonFunc);
	// callback for resizing the window
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboardFunc);
	glutMainLoop();
	return 0;
}