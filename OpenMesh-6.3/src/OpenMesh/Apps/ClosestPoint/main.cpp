#include <iostream>

#include "LoadModel.h"
#include "PushOutMesh.h"
#include "GlobalType.h"

#if defined(WIN32) || defined(linux)
  #include <GL/gl.h>
  #include <GL/glut.h>
#elif defined(__APPLE__)
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#endif

void init()
{
    glShadeModel(GL_SMOOTH);
    glClearColor(.2f, .2f, .4f, .5f);
    glEnable(GL_DEPTH_TEST);

}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glutSwapBuffers();
    glutPostRedisplay();
}
void reshape(int w, int h)  
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();  
    if (h==0)  
        gluPerspective(80,(float)w,1.0,5000.0);
    else
        gluPerspective (80,( float )w /( float )h,1.0,5000.0 );
    glMatrixMode(GL_MODELVIEW);  
    glLoadIdentity(); 
}

int
main(int argc, char* argv[])
{
    glutInit(&argc,argv);
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH ); 
    glutInitWindowSize( 1280, 720 ); 
    glutCreateWindow( "Cloth Tutorial from cg.alexandra.dk" );
    init();

    glutDisplayFunc(display);  
    glutReshapeFunc(reshape);

    MyMesh cloth, body;
    IOHelper m_helper;
    MeshExtruder m_extruder;

    // Read in mesh
    m_helper.init(argc, argv);
    m_helper.readin(0, argv, body);
    m_helper.readin(1, argv, cloth);

    // do extrude operation
    m_extruder.doPushOut(body, cloth);

    m_helper.writeto(2, argv, cloth);

    glutMainLoop();
}
