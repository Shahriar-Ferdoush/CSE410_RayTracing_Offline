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





class Object
{
    public:
        Color color;
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
            radius = 0;
        }

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
            double b = 2 * (dir * (start - center));
            double c = (start - center) * (start - center) - radius * radius;

            double d = b * b - 4 * a * c;

            if (d < 0) {
                return -1;
            }

            double t1 = (-b + sqrt(d)) / (2 * a);
            double t2 = (-b - sqrt(d)) / (2 * a);

            if (t1 < 0 && t2 < 0) {
                return -1;
            }

            if (t1 < 0) {
                return t2;
            }

            if (t2 < 0) {
                return t1;
            }

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

}


// Floor class with infinite length and width
class Floor : public Object
{
    public:
        double floorWidth, tileWidth;

        Floor() {
            floorWidth = tileWidth = 50;
        }

        Floor(double floorWidth, double tileWidth, Color color, double coEfficients[4], double shine) : floorWidth(floorWidth), tileWidth(tileWidth), Object(color, coEfficients, shine) {}

        Floor(double floorWidth, double tileWidth, Color color, double coEfficients[4], double shine, bitmap_image texture) : floorWidth(floorWidth), tileWidth(tileWidth), Object(color, coEfficients, shine, texture) {}

        void draw() {
            int tiles = floorWidth / tileWidth;

            glPushMatrix();
                glTranslatef(-floorWidth / 2, -floorWidth / 2, 0);
                for (int i = 0; i < tiles; i++) {
                    for (int j = 0; j < tiles; j++) {
                        if ((i + j) % 2 == 0) {
                            glColor3f(1, 1, 1);
                        } else {
                            glColor3f(0, 0, 0);
                        }
                        glBegin(GL_QUADS);
                            glVertex3f(i * tileWidth, j * tileWidth, 0);
                            glVertex3f((i + 1) * tileWidth, j * tileWidth, 0);
                            glVertex3f((i + 1) * tileWidth, (j + 1) * tileWidth, 0);
                            glVertex3f(i * tileWidth, (j + 1) * tileWidth, 0);
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

            return t;
        }

        Point getNormal(Point intersectionPoint) {
            if (intersectionPoint.z > 0) {
                return Point(0, 0, 1);
            } else {
                return Point(0, 0, -1);
            }
        }

        Color getColor(Point intersectionPoint) {
            if (!isTexture) {
                return color;
            }

            double u = intersectionPoint.x / tileWidth;
            double v = intersectionPoint.y / tileWidth;

            int x = u * texture.width();
            int y = v * texture.height();

            unsigned char r, g, b;
            texture.get_pixel(x, y, r, g, b);

            return Color(r, g, b);
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
};
