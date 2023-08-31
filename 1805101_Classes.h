#include <bits/stdc++.h>
#include <GL/glut.h>
#include <cmath>
#include "bitmap_image.hpp"

#define PI (2*acos(0.0))

using namespace std;


// Class decralation
class Object;
class Light;
class SpotLight;
class Point;
class Color;
class Ray;
class Sphere;
class Floor;
class Cube; 
class Pyramid;

// Extern variables
extern int recursionLevel;
extern vector<Object*> objects;
extern vector<Light*> lights;
extern vector<SpotLight*> spotLights;

extern Floor checkerBoard;
extern Sphere sphere;
extern Cube cube;
extern Pyramid pyramid;

class Point
{
    public:
        double x, y, z, w;

        // --------------------------------------------
        // Constructors
        Point() {
            x = y = z = 0.0;
            w = 1.0;
        }

        Point(double x, double y, double z) : x(x), y(y), z(z), w(1) {}

        Point(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}

        Point(const Point& p) : x(p.x), y(p.y), z(p.z), w(p.w) {}

        // --------------------------------------------
        // Operators
        Point operator=(Point p) {
            x = p.x;
            y = p.y;
            z = p.z;
            w = p.w;
            return *this;
        }

        Point operator+(Point p) {
            return Point(x + p.x, y + p.y, z + p.z);
        }

        Point operator-(Point p) {
            return Point(x - p.x, y - p.y, z - p.z);
        }

        Point operator*(double d) {
            return Point(x * d, y * d, z * d);
        }

        Point operator/(double d) {
            return Point(x / d, y / d, z / d);
        }

        // Dot product
        double operator*(Point p) {
            return x * p.x + y * p.y + z * p.z;
        }

        // Cross product
        Point operator^(Point p) {
            return Point(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
        }

        // Magnitude
        double lenght() {
            return sqrt(x * x + y * y + z * z);
        }

        // Normalize this
        void normalize() {
            double d = lenght();
            x /= d;
            y /= d;
            z /= d;
        }

        // Scale down to 3D space
        void scaleDown() {
            x /= w;
            y /= w;
            z /= w;
            w = 1.0;
        }

        // Input with operator >>
        friend istream& operator>>(istream& is, Point& p) {
            is >> p.x >> p.y >> p.z;
            return is;
        }

        // Output with operator <<
        friend ostream& operator<<(ostream& os, Point& p) {
            os << fixed << setprecision(7) << p.x << " " << p.y << " " << p.z;
            return os;
        }
};

class Color
{
    public:
        int r, g, b;

        Color() {
            // Initialize with random color
            r = rand() % 256;
            g = rand() % 256;
            b = rand() % 256;
        }

        Color(int r, int g, int b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }

        // Input with operator >>
        friend istream& operator>>(istream& is, Color& c) {
            is >> c.r >> c.g >> c.b;
            return is;
        }

        // Output with operator <<
        friend ostream& operator<<(ostream& os, Color& c) {
            os << c.r << " " << c.g << " " << c.b;
            return os;
        }
};

// Point Light
class Light
{
    public:
        Point pos;
        Color color;

        Light() {
            pos = Point(0, 0, 0);
            color = Color(255, 255, 255);
        }

        Light(Point pos, Color color) : pos(pos), color(color) {}

        void draw() {
            glPushMatrix();
                glTranslatef(pos.x, pos.y, pos.z);
                glColor3f(color.r / 255.0, color.g / 255.0, color.b / 255.0);
                glutSolidSphere(0.1, 10, 10);
            glPopMatrix();
            glEnd();
        }

        // Input with operator >>
        friend istream& operator>>(istream& is, Light& l) {
            is >> l.pos >> l.color.r >> l.color.g >> l.color.b;
            return is;
        }
};


// Spot Light
class SpotLight
{
    public:
        Light light;
        Point lookAt;
        double angle;

        SpotLight() {
            light = Light();
            lookAt = Point(0, 0, 0);
            angle = 0;
        }

        void draw() {
            light.draw();
            glBegin(GL_LINES);
                glVertex3f(light.pos.x, light.pos.y, light.pos.z);
                glVertex3f(lookAt.x, lookAt.y, lookAt.z);
            glEnd();
        }

        // Input with operator >>
        friend istream& operator>>(istream& is, SpotLight& sl) {
            is >> sl.light >> sl.lookAt >> sl.angle;
            return is;
        }
};

// Ray
class Ray
{
    public:
        Point start, dir;

        Ray() {
            start = Point(0, 0, 0);
            dir = Point(0, 0, 0);
        }

        Ray(Point start, Point dir) {
            this->start = start;

            // Normalize direction
            dir.normalize();
            this->dir = dir;
        }

        void draw() {
            glBegin(GL_LINES);
                glVertex3f(start.x, start.y, start.z);
                glVertex3f(dir.x, dir.y, dir.z);
            glEnd();
        }

        // Input with operator >>
        friend istream& operator>>(istream& is, Ray& r) {
            is >> r.start >> r.dir;
            return is;
        }

        // Output with operator <<
        friend ostream& operator<<(ostream& os, Ray& r) {
            os << fixed << setprecision(7) << "Ray : " << r.start << " " << r.dir;
            return os;
        }
};





class Object
{
    public:
        Color color;
        Point referencePoint;
        double coEfficients[4];
        double shine;
        bool isTexture;
        bitmap_image texture;

        Object() {
            color = Color();
            coEfficients[0] = coEfficients[1] = coEfficients[2] = coEfficients[3] = 0;
            shine = 0;
            isTexture = false;
        }

        Object(Color color, double coEfficients[4], double shine) : color(color), shine(shine) {
            for (int i = 0; i < 4; i++) {
                this->coEfficients[i] = coEfficients[i];
            }
            isTexture = false;
        }

        Object(Color color, double coEfficients[4], double shine, bitmap_image texture) : color(color), shine(shine), texture(texture) {
            for (int i = 0; i < 4; i++) {
                this->coEfficients[i] = coEfficients[i];
            }
            isTexture = true;
        }

        // Setter Functions
        void setColor(Color color) {
            this->color = color;
        }

        void setReferencePoint(Point referencePoint) {
            this->referencePoint = referencePoint;
        }

        void setCoEfficients(double coEfficients[4]) {
            for (int i = 0; i < 4; i++) {
                this->coEfficients[i] = coEfficients[i];
            }
        }

        void setShine(double shine) {
            this->shine = shine;
        }

        void setTexture(bitmap_image texture) {
            this->texture = texture;
            isTexture = true;
        }

        virtual void draw() {}

        virtual double getIntersectingT(Ray ray) {}

        virtual Point getNormal(Point intersectionPoint) {}

        virtual Color getColor(Point intersectionPoint) {}

        virtual void print() {}

        // Input with operator >>
        friend istream& operator>>(istream& is, Object& o) {
            is >> o.color.r >> o.color.g >> o.color.b;
            for (int i = 0; i < 4; i++) {
                is >> o.coEfficients[i];
            }
            is >> o.shine;
            return is;
        }
};


class Sphere : public Object
{
    public:
        Point center;
        double radius;

        Sphere() {
            center = Point(0, 0, 0);
            radius = 1;
            color = Color(255, 0, 0);
        }

        Sphere(Point center, double radius, Color color) : center(center), radius(radius), Object(color, NULL, 0) {}

        Sphere(Point center, double radius, Color color, double coEfficients[4], double shine) : center(center), radius(radius), Object(color, coEfficients, shine) {}

        Sphere(Point center, double radius, Color color, double coEfficients[4], double shine, bitmap_image texture) : center(center), radius(radius), Object(color, coEfficients, shine, texture) {}

        void draw() {
            glPushMatrix();
                glTranslatef(center.x, center.y, center.z);
                glColor3f(color.r / 255.0, color.g / 255.0, color.b / 255.0);
                glutSolidSphere(radius, 100, 100);
            glPopMatrix();
            glEnd();
        }

        double getIntersectingT(Ray ray) {
            Point start = ray.start;
            Point dir = ray.dir;

            double a = dir * dir;
            double b = 2 * (dir * start);
            double c = (start * start) - radius * radius;

            double d = b * b - 4 * a * c;

            if (d < 0) {
                return -1;
            }

            // If d >= 0, there are two roots
            double t1 = (-b + sqrt(d)) / (2 * a);
            double t2 = (-b - sqrt(d)) / (2 * a);

            // Both behind the origin
            if (t1 < 0 && t2 < 0) {
                return -1;
            }

            // One behind the origin
            if (t1 < 0) {
                return t2;
            }

            if (t2 < 0) {
                return t1;
            }

            // Both in front of the origin, return whats closer
            return min(t1, t2);
        }

        Point getNormal(Point intersectionPoint) {
            Point normal = intersectionPoint - center;
            normal.normalize();
            return normal;
        }

        Color getColor(Point intersectionPoint) {
            if (!isTexture) {
                return color;
            }

            double theta = acos((intersectionPoint.z - center.z) / radius);
            double phi = atan2(intersectionPoint.y - center.y, intersectionPoint.x - center.x);

            double u = (phi + PI) / (2 * PI);
            double v = (PI - theta) / PI;

            int x = u * texture.width();
            int y = v * texture.height();

            unsigned char r, g, b;
            texture.get_pixel(x, y, r, g, b);

            return Color(r, g, b);
        }

        void print() {
            cout << "Sphere" << endl;
            cout << "Center : " << center << endl;
            cout << "Radius : " << radius << endl;
            cout << "Color : " << color << endl;
            cout << "Co-efficients : ";
            for (int i = 0; i < 4; i++) {
                cout << coEfficients[i] << " ";
            }
            cout << endl;
            cout << "Shine : " << shine << endl;
        }
};


// Checkerboard floor

class Floor : public Object
{
    public:
        double floorWidth, tileWidth;

        Floor() {
            floorWidth = 10;
            tileWidth = 1;
        }

        Floor(double FloorWidth) {
            floorWidth = FloorWidth;
            tileWidth = 1;
        }

        Floor(double FloorWidth, Color color, double coEfficients[4], double shine) : floorWidth(FloorWidth), tileWidth(1), Object(color, coEfficients, shine) {}

        Floor(double floorWidth, double tileWidth, Color color, double coEfficients[4], double shine) : floorWidth(floorWidth), tileWidth(tileWidth), Object(color, coEfficients, shine) {}

        Floor(double floorWidth, double tileWidth, Color color, double coEfficients[4], double shine, bitmap_image texture) : floorWidth(floorWidth), tileWidth(tileWidth), Object(color, coEfficients, shine, texture) {}

        void draw() {
            int tiles = floorWidth / tileWidth;

            glPushMatrix();
                glTranslatef(-floorWidth / 2, 0, -floorWidth / 2);
                for (int i = 0; i < tiles; i++) {
                    for (int j = 0; j < tiles; j++) {
                        if ((i + j) % 2 == 0) {
                            glColor3f(1, 1, 1);
                        } else {
                            glColor3f(0, 0, 0);
                        }

                        glBegin(GL_QUADS);
                            // Plane on Y = 0
                            glVertex3f(i * tileWidth, 0, j * tileWidth);
                            glVertex3f((i + 1) * tileWidth, 0, j * tileWidth);
                            glVertex3f((i + 1) * tileWidth, 0, (j + 1) * tileWidth);
                            glVertex3f(i * tileWidth, 0, (j + 1) * tileWidth);
                        glEnd();
                    }
                }
            glPopMatrix();
        }

        double getIntersectingT(Ray ray) {
            Point start = ray.start;
            Point dir = ray.dir;

            double t = -start.z / dir.z;

            if (t < 0) {
                return -1;
            }

            // calculate the intersection point
            Point intersectionPoint = start + dir * t;

            // check if the intersection point is within the floor
            if (intersectionPoint.x < 0 || intersectionPoint.x > floorWidth || intersectionPoint.y < 0 || intersectionPoint.y > floorWidth) {
                return -1;
            }

            return t;
        }

        Point getNormal(Point intersectionPoint) {
            if (intersectionPoint.z >= 0) {
                return Point(0, 0, 1);
            } else {
                return Point(0, 0, -1);
            }
        }

        Color getColor(Point intersectionPoint) {
            // Find if the point is in black or white tile
            int x = intersectionPoint.x / tileWidth;
            int y = intersectionPoint.y / tileWidth;

            if ((x + y) % 2 == 0) {
                return Color(1, 1, 1);
            } else {
                return Color(0, 0, 0);
            }
        }

        void print() {
            cout << "Floor" << endl;
            cout << "Floor Width : " << floorWidth << endl;
            cout << "Tile Width : " << tileWidth << endl;
            cout << "Color : " << color << endl;
            cout << "Co-efficients : ";
            for (int i = 0; i < 4; i++) {
                cout << coEfficients[i] << " ";
            }
            cout << endl;
            cout << "Shine : " << shine << endl;
        }

        // Input Operator Overloading
        // Input Signature
        // 50			width of each chell of the checkerboard
        // 0.1 0.1 0.8		ambient, diffuse, reflection 
        
        friend istream& operator>>(istream &input, Floor &floor) {
            input >> floor.floorWidth;

            for (int i = 0; i < 4; i++) {
                if ( i == 2) {
                    continue;
                }
                input >> floor.coEfficients[i];
            }

            return input;
        }
};



class Cube : public Object {
    // This is the cube info input signature 
    // cube
    // -100 -100 10		bottom lower left point
    // 40.0			side
    // 0.0 0.5 1.0		color
    // 0.15 0.1 0.4 0.45	ambient diffuse specular reflection coefficient
    // 10			shininess

    public:
        Point lowerLeft;
        double side;

        Cube() {
            lowerLeft = Point(0, 0, 0);
            side = 1;
            color = Color();
        }

        Cube(Point lowerLeft, double side, Color color) : lowerLeft(lowerLeft), side(side), Object(color, NULL, 0) {}

        Cube(Point lowerLeft, double side, Color color, double coEfficients[4], double shine) : lowerLeft(lowerLeft), side(side), Object(color, coEfficients, shine) {}

        Cube(Point lowerLeft, double side, Color color, double coEfficients[4], double shine, bitmap_image texture) : lowerLeft(lowerLeft), side(side), Object(color, coEfficients, shine, texture) {}

        void draw() {
            glPushMatrix();
                glTranslatef(lowerLeft.x, lowerLeft.y, lowerLeft.z);
                glColor3f(color.r / 255.0, color.g / 255.0, color.b / 255.0);
                glutSolidCube(side);
            glPopMatrix();
            glEnd();
        }

        double getIntersectingT(Ray ray) {
            Point start = ray.start;
            Point dir = ray.dir;

            double t1 = (lowerLeft.x - start.x) / dir.x;
            double t2 = (lowerLeft.x + side - start.x) / dir.x;
            double t3 = (lowerLeft.y - start.y) / dir.y;
            double t4 = (lowerLeft.y + side - start.y) / dir.y;
            double t5 = (lowerLeft.z - start.z) / dir.z;
            double t6 = (lowerLeft.z + side - start.z) / dir.z;

            double tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
            double tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

            if (tmax < 0) {
                return -1;
            }

            if (tmin > tmax) {
                return -1;
            }

            return tmin;
        }

        Point getNormal(Point intersectionPoint) {
            Point normal;

            if (intersectionPoint.x == lowerLeft.x) {
                normal = Point(-1, 0, 0);
            } else if (intersectionPoint.x == lowerLeft.x + side) {
                normal = Point(1, 0, 0);
            } else if (intersectionPoint.y == lowerLeft.y) {
                normal = Point(0, -1, 0);
            } else if (intersectionPoint.y == lowerLeft.y + side) {
                normal = Point(0, 1, 0);
            } else if (intersectionPoint.z == lowerLeft.z) {
                normal = Point(0, 0, -1);
            } else if (intersectionPoint.z == lowerLeft.z + side) {
                normal = Point(0, 0, 1);
            }
        }

        Color getColor(Point intersectionPoint) {
            if (!isTexture) {
                return color;
            }

            double u, v;
            if (intersectionPoint.x == lowerLeft.x) {
                u = (intersectionPoint.z - lowerLeft.z) / side;
                v = (intersectionPoint.y - lowerLeft.y) / side;
            } else if (intersectionPoint.x == lowerLeft.x + side) {
                u = (intersectionPoint.z - lowerLeft.z) / side;
                v = (intersectionPoint.y - lowerLeft.y) / side;
            } else if (intersectionPoint.y == lowerLeft.y) {
                u = (intersectionPoint.x - lowerLeft.x) / side;
                v = (intersectionPoint.z - lowerLeft.z) / side;
            } else if (intersectionPoint.y == lowerLeft.y + side) {
                u = (intersectionPoint.x - lowerLeft.x) / side;
                v = (intersectionPoint.z - lowerLeft.z) / side;
            } else if (intersectionPoint.z == lowerLeft.z) {
                u = (intersectionPoint.x - lowerLeft.x) / side;
                v = (intersectionPoint.y - lowerLeft.y) / side;
            } else if (intersectionPoint.z == lowerLeft.z + side) {
                u = (intersectionPoint.x - lowerLeft.x) / side;
                v = (intersectionPoint.y - lowerLeft.y) / side;
            }

            int x = u * texture.width();
            int y = v * texture.height();

            unsigned char r, g, b;
            texture.get_pixel(x, y, r, g, b);

            return Color(r, g, b);
        }

        void print() {
            cout << "Cube" << endl;
            cout << "Lower Left : " << lowerLeft << endl;
            cout << "Side : " << side << endl;
            cout << "Color : " << color << endl;
            cout << "Co-efficients : ";
            for (int i = 0; i < 4; i++) {
                cout << coEfficients[i] << " ";
            }
            cout << endl;
            cout << "Shine : " << shine << endl;
        }
};

class Pyramid : public Object {
    // This is the pyramid info input signature
    // pyramid
    // -40.0 0.0 5.0    	lowest point co-ordinate
    // 30.0 40.0        	width height
    // 1.0 0.0 0.0      	color
    // 0.4 0.2 0.0 0.4  	ambient diffuse specular reflection coefficient
    // 1		 	shininess

    public:
        Point lowerLeft;
        double width, height;

        Pyramid() {
            lowerLeft = Point(0, 0, 0);
            width = height = 1;
            color = Color();
        }

        Pyramid(Point lowerLeft, double width, double height, Color color) : lowerLeft(lowerLeft), width(width), height(height), Object(color, NULL, 0) {}

        Pyramid(Point lowerLeft, double width, double height, Color color, double coEfficients[4], double shine) : lowerLeft(lowerLeft), width(width), height(height), Object(color, coEfficients, shine) {}

        Pyramid(Point lowerLeft, double width, double height, Color color, double coEfficients[4], double shine, bitmap_image texture) : lowerLeft(lowerLeft), width(width), height(height), Object(color, coEfficients, shine, texture) {}

        void draw() {
            glPushMatrix();
                glTranslatef(lowerLeft.x, lowerLeft.y, lowerLeft.z);
                glColor3f(color.r / 255.0, color.g / 255.0, color.b / 255.0);
                glBegin(GL_TRIANGLES);
                    glColor3f(1, 0, 0);
                    glVertex3f(0, 0, 0);
                    glVertex3f(width, 0, 0);
                    glVertex3f(width / 2, height, width / 2);

                    glColor3f(0, 1, 0);
                    glVertex3f(0, 0, 0);
                    glVertex3f(0, 0, width);
                    glVertex3f(width / 2, height, width / 2);

                    glColor3f(0, 0, 1);
                    glVertex3f(0, 0, width);
                    glVertex3f(width, 0, width);
                    glVertex3f(width / 2, height, width / 2);

                    glColor3f(1, 1, 0);
                    glVertex3f(width, 0, 0);
                    glVertex3f(width, 0, width);
                    glVertex3f(width / 2, height, width / 2);
                glEnd();

                glBegin(GL_QUADS);
                    glColor3f(1, 0, 1);
                    glVertex3f(0, 0, 0);
                    glVertex3f(width, 0, 0);
                    glVertex3f(width, 0, width);
                    glVertex3f(0, 0, width);
                glEnd();

            glPopMatrix();
            glEnd();
        }

        double getIntersectingT(Ray ray) {
            Point start = ray.start;
            Point dir = ray.dir;

            double t1 = (lowerLeft.x - start.x) / dir.x;
            double t2 = (lowerLeft.x + width - start.x) / dir.x;
            double t3 = (lowerLeft.y - start.y) / dir.y;
            double t4 = (lowerLeft.y + width - start.y) / dir.y;
            double t5 = (lowerLeft.z - start.z) / dir.z;
            double t6 = (lowerLeft.z + width - start.z) / dir.z;

            double tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
            double tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

            if (tmax < 0) {
                return -1;
            }

            if (tmin > tmax) {
                return -1;
            }

            return tmin;
        }

        Point getNormal(Point intersectionPoint) {
            Point normal;

            // calculate the normal on the intersection point

        }
};

