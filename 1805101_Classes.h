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
