// #include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include "Cubesphere.cpp"

struct point {
    GLfloat x, y, z;
};

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black and opaque
    glEnable(GL_DEPTH_TEST); //enable depth testing for z-culling
}


// Global variables
GLfloat eyex = 4, eyey = 4, eyez = 4;
GLfloat centerx = 0, centery = 0, centerz = 0;
GLfloat upx = 0, upy = 1, upz = 0;

GLfloat rotateAngle = 0.0f;




// Sphere Radius
GLfloat r_max = 1 / sqrt(3);

// Global varibales for scaling
GLfloat scale = 1.0f;
GLfloat radius = 0.0f; // (1 - scale) * r_max;
GLfloat cylider_lenght = sqrt(2);

GLfloat r = 1.0f;
GLfloat theta = 0.0f;
GLfloat thetaX = 0.0f;
GLfloat z = 0.0f;
GLfloat x = 0.0f;

void drawAxes() {
    glLineWidth(3);
    glBegin(GL_LINES);
    glLineWidth(3);
        //x axis
        glColor3f(1, 0, 0);
        glVertex3f(-25, 0, 0);
        glVertex3f(25, 0, 0);

        //y axis
        glColor3f(0, 1, 0);
        glVertex3f(0, -10, 0);
        glVertex3f(0, 10, 0);

        //z axis
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, -25);
        glVertex3f(0, 0, 25);
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
            glVertex3f(i, 0, -90);
            glVertex3f(i, 0, 90);

            //lines parallel to X-axis
            glVertex3f(-90, 0, i);
            glVertex3f(90, 0, i);
        }
    glEnd();
}


void drawSingleFace(GLfloat r) {
    Cubesphere c(r, 5, true);
    c.drawFace(0);
}


void drawAllSphereFaces(GLfloat r) {
    glPushMatrix();
        glColor3f(1, 0, 0);
        drawSingleFace(r);

        glRotatef(90, 0, 1, 0);
        glColor3f(0, 1, 0);
        drawSingleFace(r);

        glRotatef(90, 0, 1, 0);
        glColor3f(1, 0, 0);
        drawSingleFace(r);

        glRotatef(90, 0, 1, 0);
        glColor3f(0, 1, 0);
        drawSingleFace(r);

        glRotatef(90, 0, 0 ,1);
        glColor3f(0, 0, 1);
        drawSingleFace(r);

        glRotatef(180, 0, 0, 1);
        glColor3f(0, 0, 1);
        drawSingleFace(r);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);

    drawAxes();
    drawGrid();
    scale = 0.6f;
    glScalef(scale, scale, scale);

    glTranslatef(0, 1, 0);
    glPushMatrix();
        glTranslatef(x, 0, z);
        glRotatef(theta, 1, 0, 0);
        drawAllSphereFaces(r);
    glPopMatrix();


        

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

        case 'a':
            // rotate the sphere forward (clockwise)
            theta += 5;
            // translate by circumference
            z = theta * 2 * M_PI / 360;
            break;
        case 'd':
            // rotate the sphere backward (counter-clockwise)
            theta -= 5;
            // translate by circumference
            z = theta * 2 * M_PI / 360;
            break;

        case 'w':  
            // rotate the sphere left (counter-clockwise)
            thetaX += 5;
            // translate by circumference
            x = thetaX * 2 * M_PI / 360;
            break;

        case 's':
            // rotate the sphere right (clockwise)
            thetaX -= 5;
            // translate by circumference
            x = thetaX * 2 * M_PI / 360;
            break;
    }

    glutPostRedisplay();
}

void specialKeyListener(int key, int x, int y) {
    const GLfloat translationSpeed = 0.1f;

    switch (key) {
        case GLUT_KEY_UP: // Move Forward
            eyex -= upx * translationSpeed;
            eyey -= upy * translationSpeed;
            eyez -= upz * translationSpeed;
            break;
        case GLUT_KEY_DOWN: // Move Backward
            eyex += upx * translationSpeed;
            eyey += upy * translationSpeed;
            eyez += upz * translationSpeed;
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
