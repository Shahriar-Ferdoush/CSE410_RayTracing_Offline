#include <bits/stdc++.h>
#include <GL/glut.h>
#include <cmath>
#include "bitmap_image.hpp"

#define PI (2*acos(0.0))

using namespace std;

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

