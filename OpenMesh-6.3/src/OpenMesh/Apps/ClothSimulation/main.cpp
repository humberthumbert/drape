#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Utils/getopt.h>
#include <stdio.h>

//--------lib

#if defined(WIN32) || defined(linux)
  #include <GL/gl.h>
  #include <GL/glut.h>
#elif defined(__APPLE__)
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#endif

#include "GlobalTypes.h"
#include "Cloth.h"
#include "KDTree.h"
#include "Physics.h"
#include "Vector.h"

static const int row = 50;
static const int col = 50;

using namespace OpenMesh;

MyMesh myMesh;
Rigidbody mBody;
Cloth *mCloth;

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

    myMesh.request_vertex_normals();
    myMesh.request_face_normals();
    IO::Options myOption = optind;
    myOption+= IO::Options::VertexNormal;
    myOption+= IO::Options::FaceNormal;
    std::cout<<"VN? "<< myOption.check(IO::Options::VertexNormal) <<std::endl;
    // myOption+= IO::Options::VertexTexCoord;
    bool success = IO::read_mesh( myMesh, "~/Perfit_3D_cloth-master/OpenMesh-6.3/src/OpenMesh/Apps/BodySimulation/models/AliStand.obj", myOption);
    myMesh.update_normals();
    std::cout<<success<<std::endl;

    //Initializtin collision check;
    mBody.init(myMesh);
}

void display()
{
    //std::cout<<"Before Display"<<std::endl;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);  
    glLoadIdentity(); 
    gluLookAt(100, 0, -100, 0, 0, 0, 0, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glColor3f(0.5, 0.5, 0.0);
    static Vector3 gravity(0, -9.8, 0);
    mCloth->addForceToAll(gravity);
    mCloth->timeStep(0.01);
    //mCloth->printAllParticles();
    std::vector<Face> faces = mCloth->getFaces();
    for (unsigned int x = 0; x < faces.size(); ++x)
    {
        glVertex3f(faces[x].points[0]->getPos().m_x, faces[x].points[0]->getPos().m_y, faces[x].points[0]->getPos().m_z);
        glVertex3f(faces[x].points[1]->getPos().m_x, faces[x].points[1]->getPos().m_y, faces[x].points[1]->getPos().m_z);
        glVertex3f(faces[x].points[2]->getPos().m_x, faces[x].points[2]->getPos().m_y, faces[x].points[2]->getPos().m_z);
    }
    glEnd(); 



    glutSwapBuffers();
    //std::cout<<"After Display"<<std::endl;

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

int
main(int argc, char* argv[])
{
    std::pair<float, float> **points = new std::pair<float, float> *[row];
    for (unsigned int i = 0; i < row; ++i)
    {
        points[i] = new std::pair<float, float>[col];
        for (unsigned int j = 0; j < col; ++j)
        {
            points[i][j] = std::make_pair(i, j);
        }
    }

    mCloth = new Cloth(points, row, col);
    
    for (unsigned int i = 0; i < row; ++i)
    {
        delete [] points[i];
    }

    mCloth->getParticle(25, 25)->setMovable(false);
    // mCloth->getParticle(0, 1)->setMovable(false);
    // mCloth->getParticle(1, 0)->setMovable(false);
    // mCloth->getParticle(49, 49)->setMovable(false);
    // mCloth->getParticle(48, 49)->setMovable(false);
    // mCloth->getParticle(49, 48)->setMovable(false);

    glutInit(&argc,argv);
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH ); 
    glutInitWindowSize( 1280, 720 ); 
    glutCreateWindow( "Cloth Tutorial from cg.alexandra.dk" );
    init();

    glutDisplayFunc(display);  
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardFunc);
    glutIdleFunc(idleFunc);
    glutMainLoop();

    delete mCloth;
}
