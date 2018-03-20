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
#include "Cloth.h"
// ----------------------------------------------------------------------------
//typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;
// ----------------------------------------------------------------------------
// Build a simple cube and write it to std::cout
	
MyMesh mesh;
QuadMesh clothmesh;
Body* body;
Cloth* cloth;
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
void translate(float& x, float& y, float& z){
	x += landTranslate[0];
	y += landTranslate[1];
	z += landTranslate[2];
}

void position(float& x, float& y, float& z){
	MyMesh::Point rightshoulder = body->getLandmarks()[4];
	x = rightshoulder[0] - x + 0.05, y = rightshoulder[1] - y + 0.05, z = rightshoulder[2] - z + 0.05;
}
bool first = false;
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);  
    glLoadIdentity(); 
    gluLookAt(0, 0, 4, 0, 0, 0, 0, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    // Print all the faces
    glBegin(GL_TRIANGLES);

    glColor3f(1.0, 1.0, 0.0);
	static Vector3 gravity(0, -0.1, 0);
    cloth->addForceToAll(gravity);

   if (first){
	    cloth->timeStep(0.001);
		first = false;    	
    }
    
    std::vector<Face> faces = cloth->getFaces();
    for (unsigned int i = 0; i < faces.size(); ++i)
    {
    	for(unsigned int j = 0; j < 3; ++j){

    		float x = faces[i].points[j]->getPos().m_x, y = faces[i].points[j]->getPos().m_y, z = faces[i].points[j]->getPos().m_z;
    		translate(x, y, z);
    		rotate(x, y, z);
    		glVertex3f(x, y, z);

    	}
    }
/*    
	for(MyMesh::FaceIter f_it = clothmesh.faces_begin(); f_it != clothmesh.faces_end(); ++f_it) {
		std::cout << "found face" << std::endl;
		int count = 0;
		for (MyMesh::FaceVertexIter fv_it=clothmesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
  		{
  			std::cout << "found face vertex" << std::endl;
  			if(count == 3){

  				count = 0;
  			}
  			else{
  				float x = clothmesh.point(*fv_it)[0]; float y = clothmesh.point(*fv_it)[1]; float z = clothmesh.point(*fv_it)[2];
	    	translate(x, y , z);
	    	rotate(x, y, z);
			glVertex3f(x, y, z);
			count++;    
  			}
	    		
		}
    }*/
    glColor3f(0.0, 1.0, 0.0);
    mesh = body->getBody();
	for(MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
		
		for (MyMesh::FaceVertexIter fv_it=mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
  		{
	    	// do something with e.g. mesh.point(*fv_it)
	    	float x = mesh.point(*fv_it)[0]; float y = mesh.point(*fv_it)[1]; float z = mesh.point(*fv_it)[2];
	    	translate(x, y , z);
	    	rotate(x, y, z);
			glVertex3f(x, y, z);    	
		}
    } 
    glEnd();
	

    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    std::vector<MyMesh::Point> landmarks = body->getLandmarks();
    for(unsigned i = 0; i < landmarks.size();i++){
	    float x = landmarks[i][0], y = landmarks[i][1], z = landmarks[i][2];
	    translate(x, y , z);
	    rotate(x, y, z);
	    glPushMatrix();
    	    glTranslated(x,y,z);
        	glutSolidSphere(0.04,50,50);
    	glPopMatrix();
	}
	float x = 0.558488, y = 4.922700, z = -0.124880;
	translate(x,y,z);
	rotate(x,y,z);
	glPushMatrix();
        glTranslated(x,y,z);
       	glutSolidSphere(0.04,50,50);
    glPopMatrix();
	/*for(unsigned i = 0; i < body->getXSegment(20).size(); i++){
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
        case 's':
        	first = true;
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
	if (argc!=3)
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
	OpenMesh::IO::Options opt, clothopt;
	if ( ! OpenMesh::IO::read_mesh(mesh,argv[1], opt))
	{
    	std::cerr << "Error loading mesh from file " << argv[1] << std::endl;
		return 1;
	} 

	else if ( ! OpenMesh::IO::read_mesh(clothmesh, argv[2], clothopt))
	{
		std::cerr << "Error loading cloth mesh fromt file " << argv[2] << std::endl;
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
	} else 	if ( !clothopt.check( OpenMesh::IO::Options::VertexNormal ) )
	{
    	clothmesh.request_face_normals();
    	clothmesh.update_normals();
    	clothmesh.release_face_normals();
	}
	
/*
  QuadMesh::VertexHandle vhandle[4];
  vhandle[0] = clothmesh.add_vertex(MyMesh::Point(-1, 3,  -1));
  vhandle[1] = clothmesh.add_vertex(MyMesh::Point( 1, 3,  -1));
  vhandle[2] = clothmesh.add_vertex(MyMesh::Point( 1, 3,  1));
  vhandle[3] = clothmesh.add_vertex(MyMesh::Point(-1, 3,  1));

  // generate (quadrilateral) faces
  std::vector<MyMesh::VertexHandle>  face_vhandles;
  face_vhandles.clear();
  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[2]);
  face_vhandles.push_back(vhandle[3]);
  clothmesh.add_face(face_vhandles);
*/
	// Body
	body = new Body(mesh, 183);
	// Cloth
	cloth = new Cloth(clothmesh);
    /*float disx = 0.558488, disy = 4.922700, disz = -0.124880;
	position(disx, disy, disz);
	for (MyMesh::VertexIter v_it=clothmesh.vertices_begin(); v_it!=clothmesh.vertices_end(); ++v_it)
	{
		clothmesh.point(*v_it)[0]+=disx;
		clothmesh.point(*v_it)[1]+=disy;
		clothmesh.point(*v_it)[2]+=disz;
	}*/

	// don't need the normals anymore? Remove them!
	mesh.release_vertex_normals();
	clothmesh.release_vertex_normals();
	// just check if it really works
	if (mesh.has_vertex_normals())
	{
		std::cerr << "Ouch! ERROR! Shouldn't have any vertex normals anymore!\n";
	 	return 1;
	} else if (clothmesh.has_vertex_normals()){
		std::cerr << "Cloth: Ouch! ERROR! Shouldn't have any vertex normals anymore!\n";
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