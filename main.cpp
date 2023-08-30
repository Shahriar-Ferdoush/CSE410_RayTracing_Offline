// #include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include "Cubesphere.cpp"
#include "1805101_Classes.h"

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black and opaque
    glEnable(GL_DEPTH_TEST); //enable depth testing for z-culling
}


// Ray Tracing

Floor checkerBoard(50);
Sphere sphere;
Pyramid pyramid;
Cube cube;

// Global variables
GLfloat eyex = 4, eyey = 4, eyez = 4;
GLfloat centerx = 0, centery = 0, centerz = 0;
GLfloat upx = 0, upy = 1, upz = 0;

GLfloat rotateAngle = 0.0f;




// Sphere Radius
GLfloat r_max = 1 / sqrt(3);


void drawAxes() {
    glLineWidth(3);
    glBegin(GL_LINES);
    glLineWidth(3);
        //x axis
        glColor3f(1, 0, 0);
        glVertex3f(-100, 0, 0);
        glVertex3f(100, 0, 0);

        //y axis
        glColor3f(0, 1, 0);
        glVertex3f(0, -100, 0);
        glVertex3f(0, 100, 0);

        //z axis
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, -100);
        glVertex3f(0, 0, 100);
    glEnd();
}

void drawGrid() {
    int i;
    glColor3f(0.6, 0.6, 0.6); //grey
    glBegin(GL_LINES);
        for(i = -25; i <= 25; i++) {
            if(i == 0)
                continue; //SKIP the MAIN axes

            //lines parallel to Z-axis
            glVertex3f(i, 0, -25);
            glVertex3f(i, 0, 25);

            //lines parallel to X-axis
            glVertex3f(-25, 0, i);
            glVertex3f(25, 0, i);
        }
    glEnd();
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);

    // Draw Axes and Grid
    glPushMatrix();
        drawAxes();
        drawGrid();
    glPopMatrix();

    // Draw a Floor
    // checkerBoard.draw();

    // Draw Shpere using 1805101_Classes.h
    // sphere.draw();

    // Draw Pyramid using 1805101_Classes.h
    // pyramid.draw();

    // Draw Cube using 1805101_Classes.h
    // cube.draw();




        

    glutSwapBuffers();
}

void reshapeListener(GLsizei width, GLsizei height) {
    if (height == 0) height = 1;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);

}

void keyboardListener(unsigned char key, int x, int y) {
    const GLfloat translationSpeed = 0.1f;

    switch (key) {
        case 27: // ESC key
            exit(0);
            break;
        case '1': // Rotate/Look Left
            eyex -= translationSpeed;
            break;
        case '2': // Rotate/Look Right
            eyex += translationSpeed;
            break;
        case '3': // Look Up
            eyey += translationSpeed;
            break;
        case '4': // Look Down
            eyey -= translationSpeed;
            break;
        case '5': // Tilt Counterclockwise
            upz -= translationSpeed;
            break;
        case '6': // Tilt Clockwise
            upz += translationSpeed;
            break;
    }

    glutPostRedisplay();
}

void specialKeyListener(int key, int x, int y) {
    const GLfloat translationSpeed = 0.1f;

    switch (key) {
        case GLUT_KEY_UP: // Move Forward
            eyex -= upx * translationSpeed * 10;
            eyey -= upy * translationSpeed * 10;
            eyez -= upz * translationSpeed * 10;
            break;
        case GLUT_KEY_DOWN: // Move Backward
            eyex += upx * translationSpeed * 10;
            eyey += upy * translationSpeed * 10;
            eyez += upz * translationSpeed * 10;
            break;
        case GLUT_KEY_RIGHT: // Move Right
            eyex += translationSpeed;
            centerx += translationSpeed;
            break;
        case GLUT_KEY_LEFT: // Move Left
            eyex -= translationSpeed;
            centerx -= translationSpeed;
            break;
        case GLUT_KEY_PAGE_UP: // Move Up
            eyey += translationSpeed;
            centery += translationSpeed;
            break;
        case GLUT_KEY_PAGE_DOWN: // Move Down
            eyey -= translationSpeed;
            centery -= translationSpeed;
            break;
    }

    glutPostRedisplay();
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(640, 640);
    glutInitWindowPosition(50, 50);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("RayTracing");
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeListener);
    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    initGL();
    glutMainLoop();
    return 0;
}
