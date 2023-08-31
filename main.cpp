// #include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include "Cubesphere.cpp"
#include "1805101_Classes.h"

using namespace std;

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black and opaque
    glEnable(GL_DEPTH_TEST); //enable depth testing for z-culling
}


// Ray Tracing

Floor checkerBoard;
Sphere mySphere;
Pyramid myPyramid;
Cube myCube;

// Global Points, eye position, look forward, up vector, right vector
Point eye(-200, -50, -200);
Point look(-1 / sqrt(2), 0, -1 / sqrt(2));
Point up(0, 1, 0);
Point rightVector(-1 / sqrt(2), 0, 1 / sqrt(2));


double rotateAngle = 2*M_PI/180;


// --------------------------------------------------------

// Ray Tracing Global Variables
double near, far;
double fovY;
double aspectRatio;

int recursionLevel;
int noOfPixels;

int noOfObjects;


void rotate( Point &p, Point &axis, double angle ) {
    p = p * cos(angle) + (axis ^ p) * sin(angle);
}

void takeInputs() {
    // Input from description.txt
    ifstream is;
    is.open("description.txt");

    is >> near >> far;
    is >> fovY;
    is >> aspectRatio;

    is >> recursionLevel;
    is >> noOfPixels;

    // Handle Checkerbord Input
    // Floor cbFloor;
    // is >> cbFloor;

    is >> checkerBoard;
    checkerBoard.print();


    // Input Objects
    is >> noOfObjects;
    cout << "No of Objects: " << noOfObjects << endl;

    // for(int i = 0; i < noOfObjects; i++) {
    //     string objectType;
    //     is >> objectType;

    //     Object* object;
    //     if(objectType == "sphere") {
    //         object = new Sphere();
    //         is >> *((Sphere*) object);
    //     }
    //     else if(objectType == "pyramid") {
    //         object = new Pyramid();
    //         is >> *((Pyramid*) object);
    //     }
    //     else if(objectType == "cube") {
    //         object = new Cube();
    //         is >> *((Cube*) object);
    //     }
    // }

    // Input Sphere
    // sphere
    // 20.0 20.0 20.0 		center
    // 20.0 			radius
    // 1.0 1.0 0.0 		color
    // 0.04 0.03 0.03 0.9 	ambient diffuse specular reflection coefficient
    // 30 			shininess

    string objectType;
    is >> objectType;

    
    is >> mySphere;
    mySphere.print();

    is >> objectType;
    cout << objectType << "-----------------------------" << endl;
    is >> myPyramid;
    myPyramid.print();

    is >> objectType;
    is >> myCube;
    myCube.print();


    


}


void drawAxes() {
    glLineWidth(3);
    glBegin(GL_LINES);
    glLineWidth(3);
        //x axis
        glColor3f(1, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(100, 0, 0);

        //y axis
        glColor3f(0, 1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 100, 0);

        //z axis
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, 0);
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
    gluLookAt(eye.x, eye.y, eye.z, look.x, look.y, look.z, up.x, up.y, up.z);

    // Draw Axes and Grid
    glPushMatrix();
        drawAxes();
        drawGrid();
    glPopMatrix();

    // Draw a Floor
    checkerBoard.draw();

    // Draw All the Objects
    // for(int i = 0; i < noOfObjects; i++) {
    //     // Draw Object
    //     objects[i]->draw();
    // }

    mySphere.draw();
    myPyramid.draw();
    myCube.draw();




        

    glutSwapBuffers();
}

void reshapeListener(GLsizei width, GLsizei height) {
    if (height == 0) height = 1;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovY, aspectRatio, far, near);

}

void keyboardListener(unsigned char key, int x, int y) {
    const GLfloat translationSpeed = 10.0f;

    switch (key) {
        case 27: // ESC key
            exit(0);
            break;
        case '1': // Rotate/Look Left
            rotate(rightVector, up, rotateAngle);
            rotate(look, up, rotateAngle);
            break;
        case '2': // Rotate/Look Right
            rotate(rightVector, up, -rotateAngle);
            rotate(look, up, -rotateAngle);
            break;
        case '3': // Look Up
            rotate(up, rightVector, rotateAngle);
            rotate(look, rightVector, rotateAngle);
            break;
        case '4': // Look Down
            rotate(up, rightVector, -rotateAngle);
            rotate(look, rightVector, -rotateAngle);
            break;
        case '5': // Tilt Counterclockwise
            rotate(rightVector, look, rotateAngle);
            rotate(up, look, rotateAngle);
            break;
        case '6': // Tilt Clockwise
            rotate(rightVector, look, -rotateAngle);
            rotate(up, look, -rotateAngle);
            break;
    }

    glutPostRedisplay();
}

void specialKeyListener(int key, int x, int y) {
    double translationSpeed = 5.0f;

    switch (key) {
        case GLUT_KEY_UP: // Move Forward
            eye = eye + look * translationSpeed;
            break;
        case GLUT_KEY_DOWN: // Move Backward
            eye = eye - look * translationSpeed;
            break;
        case GLUT_KEY_RIGHT: // Move Right
            eye = eye + rightVector * translationSpeed;
            break;
        case GLUT_KEY_LEFT: // Move Left
            eye = eye - rightVector * translationSpeed;
            break;
        case GLUT_KEY_PAGE_UP: // Move Up
            eye = eye + up * translationSpeed;
            break;
        case GLUT_KEY_PAGE_DOWN: // Move Down
            eye = eye - up * translationSpeed;
            break;
    }

    glutPostRedisplay();
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(640, 640);
    takeInputs();
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
