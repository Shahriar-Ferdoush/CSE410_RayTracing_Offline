// #include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include "1805101_Classes.h"

using namespace std;

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black and opaque
    glEnable(GL_DEPTH_TEST); //enable depth testing for z-culling
}

// Extern Variables
vector<Object*> objects;
vector<Light*> lights;
vector<SpotLight*> spotLights;

// Ray Tracing
Floor checkerBoard;

// Global variables
Point pos;   // position of the eye
Point l;     // look/forward direction
Point r;     // right direction
Point u;     // up direction


double rotateAngle = 2*M_PI/180;


// --------------------------------------------------------

// Ray Tracing Global Variables
double near, far;
double fovY;
double aspectRatio;

int recursionLevel; // An Extern Variable
int noOfPixels;

int noOfObjects;

int noOfLights;
int noOfSpotLights;


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
    // is >> checkerBoard;
    Object* obj = new Floor();
    is >> *((Floor*) obj);

    // Push the checkerboard in the objects vector
    objects.push_back(obj);


    // Input Objects
    is >> noOfObjects;

    for(int i = 0; i < noOfObjects; i++) {
        string objectType;
        is >> objectType;

        Object* object;
        if(objectType == "sphere") {
            object = new Sphere();
            is >> *((Sphere*) object);
        }
        else if(objectType == "pyramid") {
            object = new Pyramid();
            is >> *((Pyramid*) object);
        }
        else if(objectType == "cube") {
            object = new Cube();
            is >> *((Cube*) object);
        }

        objects.push_back(object);
    }

    // Print all objects
    for(int i = 0; i < objects.size(); i++) {
        objects[i]->print();
    }

    // Input Lights
    is >> noOfLights;

    for(int i = 0; i < noOfLights; i++) {
        Light* light = new Light();
        is >> *light;
        lights.push_back(light);
    }

    // Input Spot Lights
    is >> noOfSpotLights;

    for(int i = 0; i < noOfSpotLights; i++) {
        SpotLight* spotLight = new SpotLight();
        is >> *spotLight;
        spotLights.push_back(spotLight);
    }


}

void drawAxes() {
    glPushMatrix();
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
    glPopMatrix();
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
int print = 5;
void capture() {
    // Initialze bitmap iamge as black
    bitmap_image image(noOfPixels, noOfPixels);
    image.clear();

    // Calculate nare plane size
    double nearPlaneHeight = 2 * tan((fovY * PI) / (2 * 180)) * near;
    double nearPlaneWidth = nearPlaneHeight;

    // Calculate pixel size
    double pixelSize = nearPlaneHeight / noOfPixels;
    
    // Calculate the top left corner of the image
    Point topLeft = pos + l*near - r*(nearPlaneWidth/2) + u*(nearPlaneHeight/2);

    // Calculate top left corner Pixel middle point
    topLeft = topLeft + r*(pixelSize/2) - u*(pixelSize/2);

    int nearestObjectIndex = -1;
    double nearestObjectT = INT_MAX;
    double currentObjectT;

    // Generate Ray from the eye to all pixel middle point using the top left corner
    for(int i = 0; i < noOfPixels; i++) {
        for(int j = 0; j < noOfPixels; j++) {
            Point pixelMiddle = topLeft + r*(pixelSize*j) - u*(pixelSize*i);
            Ray ray(pos, pixelMiddle - pos);
            Color color(0, 0, 0);


            nearestObjectIndex = -1;
            nearestObjectT = INT_MAX;

            for (int k = 0; k < objects.size(); k++) {
                currentObjectT = objects[k]->getIntersectingT(ray);
                if(currentObjectT > 0 && (nearestObjectIndex == -1 || currentObjectT < nearestObjectT)) {
                    nearestObjectT = currentObjectT;
                    nearestObjectIndex = k;
                }
            }

            if(nearestObjectIndex != -1) {
                Point intersection = ray.origin + ray.dir*nearestObjectT; 
                // color = objects[nearestObjectIndex]->getColor(intersection);  // ------- have to change this -------
                double intst = objects[nearestObjectIndex]->getRayTraced(ray, 1, color);
                

                image.set_pixel(j, i, color.r * 255, color.g * 255, color.b * 255);
            }

            // Set the color of the pixel
            image.set_pixel(j, i, color.r * 255, color.g * 255, color.b * 255);
        }
    }

    image.save_image("out.bmp");

}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // control viewing (or camera)
    gluLookAt(pos.x, pos.y, pos.z, pos.x+l.x, pos.y+l.y, pos.z+l.z, u.x, u.y, u.z);
    

    // Draw Axes and Grid
    drawAxes();

    // Draw a Floor
    // checkerBoard.draw();

    // Draw All the Objects
    for(int i = 0; i < objects.size(); i++) {
        objects[i]->draw();
    }

    // Draw All the Lights
    for(int i = 0; i < noOfLights; i++) {
        lights[i]->draw();
    }

    // Draw All the Spot Lights
    for(int i = 0; i < noOfSpotLights; i++) {
        spotLights[i]->draw();
    }
        
    glutSwapBuffers();
}

void reshapeListener(GLsizei width, GLsizei height) {
    if (height == 0) height = 1;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovY, aspectRatio, near, far);

}

void keyboardListener(unsigned char key, int x, int y) {
    double rate = 0.05;
	switch(key){
        
        case '0':
            capture();
            break;

		case '1':
			r.x = r.x*cos(rate)+l.x*sin(rate);
			r.y = r.y*cos(rate)+l.y*sin(rate);
			r.z = r.z*cos(rate)+l.z*sin(rate);

			l.x = l.x*cos(rate)-r.x*sin(rate);
			l.y = l.y*cos(rate)-r.y*sin(rate);
			l.z = l.z*cos(rate)-r.z*sin(rate);
			break;

        case '2':
			r.x = r.x*cos(-rate)+l.x*sin(-rate);
			r.y = r.y*cos(-rate)+l.y*sin(-rate);
			r.z = r.z*cos(-rate)+l.z*sin(-rate);

			l.x = l.x*cos(-rate)-r.x*sin(-rate);
			l.y = l.y*cos(-rate)-r.y*sin(-rate);
			l.z = l.z*cos(-rate)-r.z*sin(-rate);
			break;

        case '3':
			l.x = l.x*cos(rate)+u.x*sin(rate);
			l.y = l.y*cos(rate)+u.y*sin(rate);
			l.z = l.z*cos(rate)+u.z*sin(rate);

			u.x = u.x*cos(rate)-l.x*sin(rate);
			u.y = u.y*cos(rate)-l.y*sin(rate);
			u.z = u.z*cos(rate)-l.z*sin(rate);
			break;

        case '4':
			l.x = l.x*cos(-rate)+u.x*sin(-rate);
			l.y = l.y*cos(-rate)+u.y*sin(-rate);
			l.z = l.z*cos(-rate)+u.z*sin(-rate);

			u.x = u.x*cos(-rate)-l.x*sin(-rate);
			u.y = u.y*cos(-rate)-l.y*sin(-rate);
			u.z = u.z*cos(-rate)-l.z*sin(-rate);
			break;

        case '5':
			u.x = u.x*cos(rate)+r.x*sin(rate);
			u.y = u.y*cos(rate)+r.y*sin(rate);
			u.z = u.z*cos(rate)+r.z*sin(rate);

			r.x = r.x*cos(rate)-u.x*sin(rate);
			r.y = r.y*cos(rate)-u.y*sin(rate);
			r.z = r.z*cos(rate)-u.z*sin(rate);
			break;

        case '6':
			u.x = u.x*cos(-rate)+r.x*sin(-rate);
			u.y = u.y*cos(-rate)+r.y*sin(-rate);
			u.z = u.z*cos(-rate)+r.z*sin(-rate);

			r.x = r.x*cos(-rate)-u.x*sin(-rate);
			r.y = r.y*cos(-rate)-u.y*sin(-rate);
			r.z = r.z*cos(-rate)-u.z*sin(-rate);
			break;

		default:
			break;
	}
	glutPostRedisplay();
}

void specialKeyListener(int key, int x,int y)
{
	switch(key){
		case GLUT_KEY_UP:		//down arrow key
			pos.x+=l.x;
			pos.y+=l.y;
			pos.z+=l.z;
			break;
		case GLUT_KEY_DOWN:		// up arrow key
			pos.x-=l.x;
			pos.y-=l.y;
			pos.z-=l.z;
			break;

		case GLUT_KEY_RIGHT:
			pos.x+=r.x;
			pos.y+=r.y;
			pos.z+=r.z;
			break;
		case GLUT_KEY_LEFT :
			pos.x-=r.x;
			pos.y-=r.y;
			pos.z-=r.z;
			break;

		case GLUT_KEY_PAGE_UP:
		    pos.x+=u.x;
			pos.y+=u.y;
			pos.z+=u.z;
			break;
		case GLUT_KEY_PAGE_DOWN:
            pos.x-=u.x;
			pos.y-=u.y;
			pos.z-=u.z;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
	glutPostRedisplay();
}


int main(int argc, char** argv) {
    pos.x=0;pos.y=30;pos.z=200;

    l.x=0;l.y=0;l.z=-1;
    u.x=0;u.y=1;u.z=0;
    r.x=1;r.y=0;r.z=0;

    takeInputs();

    glutInit(&argc, argv);
    glutInitWindowSize(noOfPixels, noOfPixels);
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
