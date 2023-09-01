#include <bits/stdc++.h>
#include <GL/glut.h>
#include <cmath>
#include "bitmap_image.hpp"

#define PI (2*acos(0.0))
#define EPSILON 1e-6

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
        double length() {
            return sqrt(x * x + y * y + z * z);
        }

        // Normalize this
        void normalize() {
            double d = length();
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
        double r, g, b;

        Color() {
            // Initialize with random color 0 to 1
            r = (rand() % 256) / 256.0;
            g = (rand() % 256) / 256.0;
            b = (rand() % 256) / 256.0;
        }

        Color(int r, int g, int b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }

        // Multiply with a scalar
        Color operator*(double d) {
            return Color(r * d, g * d, b * d);
        }

        // Add with a scalar
        Color operator+(double d) {
            return Color(r + d, g + d, b + d);
        }

        // Multiply with another color
        Color operator*(Color c) {
            return Color(r * c.r, g * c.g, b * c.b);
        }

        // Add with another color
        Color operator+(Color c) {
            return Color(r + c.r, g + c.g, b + c.b);
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
    // Input signature
    // 70.0 70.0 100.0 0.000002	position of the source, falloff parameter
    public:
        Point pos;
        double falloff;
        Color color;

        Light() {
            pos = Point(0, 0, 0);
            falloff = 0;
            color = Color(1,1,1);
        }

        Light(Point pos, double falloff) {
            this->pos = pos;
            this->falloff = falloff;
            color = Color(1,1,1);
        }

        void draw() {
            glColor3f(1, 1, 1);
            glPushMatrix();
                glTranslatef(pos.x, pos.y, pos.z);
                glutSolidSphere(1, 10, 10);
            glPopMatrix();
        }

        // Input with operator >>
        friend istream& operator>>(istream& is, Light& l) {
            is >> l.pos >> l.falloff;
            return is;
        }

        // Output with operator <<
        friend ostream& operator<<(ostream& os, Light& l) {
            os << l.pos << " " << l.falloff;
            return os;
        }

        void print() {
            cout << "Light: " << pos << " " << falloff << endl;
        }
};


// Spot Light
class SpotLight
{
    // Input signature
    // -70.0 70.0 70.0 0.0000002	position of the source, falloff parameter
    // -10 10 10			point to which it is looking
    // 30				cutoff angle in degrees
    public:
        Point pos, lookAt;
        double falloff, cutoffAngle;
        Color color;

        SpotLight() {
            pos = Point(0, 0, 0);
            lookAt = Point(0, 0, 0);
            falloff = 0;
            cutoffAngle = 0;
            color = Color(1,1,1);
        }

        SpotLight(Point pos, Point lookAt, double falloff, double cutoffAngle) {
            this->pos = pos;
            this->lookAt = lookAt;
            this->falloff = falloff;
            this->cutoffAngle = cutoffAngle;
            color = Color(1,1,1);
        }

        void draw() {
            glColor3f(1, 1, 1);
            glPushMatrix();
                glTranslatef(pos.x, pos.y, pos.z);

                // Calculate the direction vector from position to lookAt point
                Point direction = lookAt - pos;

                // Calculate the rotation angles to align the cone with the direction
                double angleX = atan2(direction.y, direction.z) * 180.0 / M_PI;
                double angleY = atan2(direction.x, direction.z) * 180.0 / M_PI;

                // Apply rotations and draw the cone
                glRotated(90, 1, 0, 0);  // Rotate the cone to align with Z-axis
                glRotated(angleX, 1, 0, 0);  // Rotate around X-axis
                glRotated(angleY, 0, 1, 0);  // Rotate around Y-axis
                glutSolidCone(1, 10, 10, 10);

            glPopMatrix();
        }

        // Input with operator >>
        friend istream& operator>>(istream& is, SpotLight& l) {
            is >> l.pos >> l.falloff >> l.lookAt >> l.cutoffAngle;
            return is;
        }

        // Output with operator <<
        friend ostream& operator<<(ostream& os, SpotLight& l) {
            os << l.pos << " " << l.falloff << " " << l.lookAt << " " << l.cutoffAngle;
            return os;
        }

        void print() {
            cout << "Spotlight : " << pos << " " << falloff << " " << lookAt << " " << cutoffAngle << endl;
        }
};

// Ray
class Ray
{
    public:
        Point origin, dir;

        Ray() {
            origin = Point(0, 0, 0);
            dir = Point(0, 1, 0);
        }

        Ray(Point origin, Point dir) {
            this->origin = origin;

            // Normalize direction
            dir.normalize();
            this->dir = dir;
        }

        void draw() {
            glBegin(GL_LINES);
                glVertex3f(origin.x, origin.y, origin.z);
                glVertex3f(dir.x, dir.y, dir.z);
            glEnd();
        }

        // Input with operator >>
        friend istream& operator>>(istream& is, Ray& r) {
            is >> r.origin >> r.dir;
            return is;
        }

        // Output with operator <<
        friend ostream& operator<<(ostream& os, Ray& r) {
            os << fixed << setprecision(7) << "Ray : " << r.origin << " " << r.dir;
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

        // --------------- Setter Functions ---------------
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


        // --------------- Virutal Functions ---------------
        virtual void draw() {}

        virtual double getIntersectingT(Ray ray) {}

        virtual Point getNormal(Point intersectionPoint) {}

        virtual Color getColor(Point intersectionPoint) {}

        virtual void print() {}

        // --------------- Ray Tracing Function ---------------
        virtual double getRayTraced(Ray ray, int level, Color &color) {
            if (recursionLevel == 0) {
                color = Color(0, 0, 0);
                return -1;
            }

            double t = getIntersectingT(ray);

            if (t < 0) {
                return -1;
            }

            if (level == 0) {
                return t;
            }

            Point intersectionPoint = ray.origin + ray.dir * t;
            Color rawColor = getColor(intersectionPoint);

    
            // Ambient
            color.r = rawColor.r * coEfficients[0];
            color.g = rawColor.g * coEfficients[0];
            color.b = rawColor.b * coEfficients[0];

            // For each light source
            for (int i = 0; i < lights.size(); i++) {
                Light* light = lights[i];
                Point lightPos = light->pos;

                Point rayDirection = lightPos - intersectionPoint;
                rayDirection.normalize();

                Ray lightRay(intersectionPoint + rayDirection * EPSILON, rayDirection);
                Point normal = getNormal(intersectionPoint);

                // distance from light source and lightRay source
                double distance = (lightPos - lightRay.origin).length();

                bool isShadow = false;
                for (int j = 0; j < objects.size(); j++) {
                    double t = objects[j]->getIntersectingT(lightRay);
                    if (t > 0 && t < distance) {
                        isShadow = true;
                        break;
                    }
                }

                if (!isShadow) {
                    // Diffuse
                    double lambert = max(0.0, lightRay.dir * normal);

                    Ray reflectedRay(intersectionPoint, rayDirection - normal * 2 * (rayDirection * normal));
                    reflectedRay.dir.normalize();

                    double phong = max(0.0, reflectedRay.dir * ray.dir);

                    color.r += lambert * rawColor.r * coEfficients[1] * light->color.r + pow(phong, shine) * coEfficients[2] * light->color.r;
                    color.g += lambert * rawColor.g * coEfficients[1] * light->color.g + pow(phong, shine) * coEfficients[2] * light->color.g;
                    color.b += lambert * rawColor.b * coEfficients[1] * light->color.b + pow(phong, shine) * coEfficients[2] * light->color.b;
                }
            }

            // Spot Light
            for (int i = 0; i < spotLights.size(); i++) {
                SpotLight* spotLight = spotLights[i];
                Point lightPos = spotLight->pos;

                Point rayDirection = lightPos - intersectionPoint;
                rayDirection.normalize();

                double angle = acos((rayDirection * spotLight->lookAt) / (rayDirection.length() * spotLight->lookAt.length())) * 180 / PI;

                if(angle <= spotLight->cutoffAngle) {
                    Ray lightRay(intersectionPoint + rayDirection * EPSILON, rayDirection);
                    Point normal = getNormal(intersectionPoint);

                    // distance from light source and lightRay source
                    double distance = (lightPos - lightRay.origin).length();

                    bool isShadow = false;
                    for (int j = 0; j < objects.size(); j++) {
                        double t = objects[j]->getIntersectingT(lightRay);
                        if (t > 0 && t < distance) {
                            isShadow = true;
                            break;
                        }
                    }

                    if (!isShadow) {
                        // Diffuse
                        double lambert = max(0.0, lightRay.dir * normal);

                        Ray reflectedRay(intersectionPoint, rayDirection - normal * 2 * (rayDirection * normal));
                        reflectedRay.dir.normalize();

                        double phong = max(0.0, reflectedRay.dir * ray.dir);

                        color.r += lambert * rawColor.r * coEfficients[1] * spotLight->color.r + pow(phong, shine) * coEfficients[2] * spotLight->color.r;
                        color.g += lambert * rawColor.g * coEfficients[1] * spotLight->color.g + pow(phong, shine) * coEfficients[2] * spotLight->color.g;
                        color.b += lambert * rawColor.b * coEfficients[1] * spotLight->color.b + pow(phong, shine) * coEfficients[2] * spotLight->color.b;
                    }
                }
            }

            // Reflection
            if(level < recursionLevel) {
                Point normal = getNormal(intersectionPoint);
                Ray reflectedRay(intersectionPoint, ray.dir - normal * 2 * (ray.dir * normal));
                reflectedRay.dir.normalize();

                reflectedRay.origin = reflectedRay.origin + reflectedRay.dir * EPSILON;

                double tMin = INT_MAX;
                int nearest = -1;

                for (int i = 0; i < objects.size(); i++) {
                    double t = objects[i]->getIntersectingT(reflectedRay);
                    if (t > 0 && t < tMin) {
                        tMin = t;
                        nearest = i;
                    }
                }

                if (nearest != -1) {
                    Color reflectedColor;
                    double t = objects[nearest]->getRayTraced(reflectedRay, level + 1, reflectedColor);

                    if (t > 0 && t < INT_MAX) {
                        color.r += reflectedColor.r * coEfficients[3];
                        color.g += reflectedColor.g * coEfficients[3];
                        color.b += reflectedColor.b * coEfficients[3];
                    }
                }
            }


            return t;
        }

        Color getReflectedColor(Ray ray, int level) {
            Color color = Color(0, 0, 0);

            if (level == 0) {
                return color;
            }

            double t = getIntersectingT(ray);

            Point intersectionPoint = ray.origin + ray.dir * t;
            Color rawColor = getColor(intersectionPoint);

    
            // Ambient
            color.r = rawColor.r * coEfficients[0];
            color.g = rawColor.g * coEfficients[0];
            color.b = rawColor.b * coEfficients[0];

            // For each light source
            for (int i = 0; i < lights.size(); i++) {
                Light* light = lights[i];
                Point lightPos = light->pos;

                Point rayDirection = lightPos - intersectionPoint;
                rayDirection.normalize();

                Ray lightRay(intersectionPoint + rayDirection * EPSILON, rayDirection);
                Point normal = getNormal(intersectionPoint);

                // distance from light source and lightRay source
                double distance = (lightPos - lightRay.origin).length();

                bool isShadow = false;
                for (int j = 0; j < objects.size(); j++) {
                    double t = objects[j]->getIntersectingT(lightRay);
                    if (t > 0 && t < distance) {
                        isShadow = true;
                        break;
                    }
                }

                if (!isShadow) {
                    // Diffuse
                    double lambert = max(0.0, lightRay.dir * normal);

                    Ray reflectedRay(intersectionPoint, rayDirection - normal * 2 * (rayDirection * normal));
                    reflectedRay.dir.normalize();

                    double phong = max(0.0, reflectedRay.dir * ray.dir);

                    color.r += lambert * rawColor.r * coEfficients[1] * light->color.r + pow(phong, shine) * coEfficients[2] * light->color.r;
                    color.g += lambert * rawColor.g * coEfficients[1] * light->color.g + pow(phong, shine) * coEfficients[2] * light->color.g;
                    color.b += lambert * rawColor.b * coEfficients[1] * light->color.b + pow(phong, shine) * coEfficients[2] * light->color.b;
                }
            }

            // Spot Light
            for (int i = 0; i < spotLights.size(); i++) {
                SpotLight* spotLight = spotLights[i];
                Point lightPos = spotLight->pos;

                Point rayDirection = lightPos - intersectionPoint;
                rayDirection.normalize();

                double angle = acos((rayDirection * spotLight->lookAt) / (rayDirection.length() * spotLight->lookAt.length())) * 180 / PI;

                if(angle <= spotLight->cutoffAngle) {
                    Ray lightRay(intersectionPoint + rayDirection * EPSILON, rayDirection);
                    Point normal = getNormal(intersectionPoint);

                    // distance from light source and lightRay source
                    double distance = (lightPos - lightRay.origin).length();

                    bool isShadow = false;
                    for (int j = 0; j < objects.size(); j++) {
                        double t = objects[j]->getIntersectingT(lightRay);
                        if (t > 0 && t < distance) {
                            isShadow = true;
                            break;
                        }
                    }

                    if (!isShadow) {
                        // Diffuse
                        double lambert = max(0.0, lightRay.dir * normal);

                        Ray reflectedRay(intersectionPoint, rayDirection - normal * 2 * (rayDirection * normal));
                        reflectedRay.dir.normalize();

                        double phong = max(0.0, reflectedRay.dir * ray.dir);

                        color.r += lambert * rawColor.r * coEfficients[1] * spotLight->color.r + pow(phong, shine) * coEfficients[2] * spotLight->color.r;
                        color.g += lambert * rawColor.g * coEfficients[1] * spotLight->color.g + pow(phong, shine) * coEfficients[2] * spotLight->color.g;
                        color.b += lambert * rawColor.b * coEfficients[1] * spotLight->color.b + pow(phong, shine) * coEfficients[2] * spotLight->color.b;
                    }
                }
            }

            // Reflection
            Point normal = getNormal(intersectionPoint);
            Ray reflectedRay(intersectionPoint, ray.dir - normal * 2 * (ray.dir * normal));
            reflectedRay.dir.normalize();

            reflectedRay.origin = reflectedRay.origin + reflectedRay.dir * EPSILON;

            double tMin = INT_MAX;
            int nearest = -1;

            for (int i = 0; i < objects.size(); i++) {
                double t = objects[i]->getIntersectingT(reflectedRay);
                if (t > 0 && t < tMin) {
                    tMin = t;
                    nearest = i;
                }
            }

            if (nearest != -1) {
                Color reflectedColor;
                double t = objects[nearest]->getIntersectingT(reflectedRay);
                color = objects[nearest]->getReflectedColor(reflectedRay, level - 1);

                if (t > 0 && t < INT_MAX) {
                    color.r += reflectedColor.r * coEfficients[3];
                    color.g += reflectedColor.g * coEfficients[3];
                    color.b += reflectedColor.b * coEfficients[3];
                }
            }

            return color;
        }

};


class Sphere : public Object
{
    // input signature
    // sphere
    // 20.0 20.0 20.0 		center
    // 20.0 			radius
    // 1.0 1.0 0.0 		color
    // 0.04 0.03 0.03 0.9 	ambient diffuse specular reflection coefficient
    // 30 			shininess
    
    public:
        Point center;
        double radius;

        Sphere() {
            center = Point(0, 0, 0);
            radius = 1;
            color = Color(1, 0, 0);
        }

        Sphere(Point center, double radius, Color color) : center(center), radius(radius), Object(color, NULL, 0) {}

        Sphere(Point center, double radius, Color color, double coEfficients[4], double shine) : center(center), radius(radius), Object(color, coEfficients, shine) {}

        Sphere(Point center, double radius, Color color, double coEfficients[4], double shine, bitmap_image texture) : center(center), radius(radius), Object(color, coEfficients, shine, texture) {}

        void draw() {
            glPushMatrix();
                glTranslatef(center.x, center.y, center.z);
                glColor3f(color.r, color.g, color.b);
                glutSolidSphere(radius, 100, 100);
            glPopMatrix();
            glEnd();
        }

        double getIntersectingT(Ray ray) {
            Point origin = ray.origin - center;
            Point dir = ray.dir;

            double a = dir * dir;
            double b = 2 * (dir * origin);
            double c = (origin * origin) - radius * radius;

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

        // Input Operator Overloading
        friend istream& operator>>(istream& in, Sphere& sphere) {
            in >> sphere.center;
            in >> sphere.radius;
            in >> sphere.color;
            for (int i = 0; i < 4; i++) {
                in >> sphere.coEfficients[i];
            }
            in >> sphere.shine;
            return in;
        }
};


// Checkerboard floor

class Floor : public Object
{
    public:
        double tileWidth;
        int tileNumber;

        Floor() {
            tileWidth = 10;
            tileNumber = 100;
        }

        Floor(double tileWidth, double coEfficients[4]) {
            this->tileWidth = tileWidth;
            this->coEfficients[0] = coEfficients[0];
            this->coEfficients[1] = coEfficients[1];
            this->coEfficients[2] = coEfficients[2];
            this->coEfficients[3] = coEfficients[3];
            isTexture = false;
        }

        void draw() {
            glPushMatrix();
                this->referencePoint = Point(-tileWidth * tileNumber / 2, 0, -tileWidth * tileNumber / 2);
                for (int i = 0; i < tileNumber; i++) {
                    for (int j = 0; j < tileNumber; j++) {
                        if ((i + j) % 2 == 0) {
                            glColor3f(1, 1, 1);
                        } else {
                            glColor3f(0, 0, 0);
                        }
                        glBegin(GL_QUADS);
                            glVertex3f(referencePoint.x + i * tileWidth, referencePoint.y, referencePoint.z + j * tileWidth);
                            glVertex3f(referencePoint.x + (i + 1) * tileWidth, referencePoint.y, referencePoint.z + j * tileWidth);
                            glVertex3f(referencePoint.x + (i + 1) * tileWidth, referencePoint.y, referencePoint.z + (j + 1) * tileWidth);
                            glVertex3f(referencePoint.x + i * tileWidth, referencePoint.y, referencePoint.z + (j + 1) * tileWidth);
                        glEnd();
                    }
                }
            glPopMatrix();
        }

        double getIntersectingT(Ray ray) {
            // y is the up direction
            Point origin = ray.origin;
            Point dir = ray.dir;

            double t = -origin.y / dir.y;

            if (t < 0) {
                return -1;
            }

            // calculate the intersection point
            Point intersectionPoint = origin + dir * t;

            // check if the intersection point is within the floor
            if (intersectionPoint.x < -tileWidth * tileNumber / 2 || intersectionPoint.x > tileWidth * tileNumber / 2 ||
                intersectionPoint.z < -tileWidth * tileNumber / 2 || intersectionPoint.z > tileWidth * tileNumber / 2) {
                return -1;
            }

            return t;
        }

        Point getNormal(Point intersectionPoint) {
            // if (intersectionPoint.y >= 0) {
            //     return Point(0, 1, 0);
            // } else {
            //     return Point(0, -1, 0);
            // }
            return Point(0, 1, 0);
        }

        Color getColor(Point intersectionPoint) {
            // Find if the point is in black or white tile
            int x = (intersectionPoint.x + tileWidth * tileNumber / 2) / tileWidth;
            int z = (intersectionPoint.z + tileWidth * tileNumber / 2) / tileWidth;

            if ((x + z) % 2 == 0) {
                return Color(1, 1, 1);
            } else {
                return Color(0, 0, 0);
            }
        }

        void print() {
            cout << "Floor" << endl;
            cout << "Tile Width : " << tileWidth << endl;
            cout << "Color : " << color << endl;
            cout << "Co-efficients : ";
            for (int i = 0; i < 4; i++) {
                cout << coEfficients[i] << " ";
            }
            cout << endl;
        }

        // Input Operator Overloading
        // Input Signature
        // 50			width of each chell of the checkerboard
        // 0.1 0.1 0.8		ambient, diffuse, reflection 
        
        friend istream& operator>>(istream &input, Floor &floor) {
            input >> floor.tileWidth;

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
                glColor3f(color.r, color.g, color.b );
                glutSolidCube(side);
            glPopMatrix();
            glEnd();
        }

        double getIntersectingT(Ray ray) {
            Point origin = ray.origin;
            Point dir = ray.dir;

            double t1 = (lowerLeft.x - origin.x) / dir.x;
            double t2 = (lowerLeft.x + side - origin.x) / dir.x;
            double t3 = (lowerLeft.y - origin.y) / dir.y;
            double t4 = (lowerLeft.y + side - origin.y) / dir.y;
            double t5 = (lowerLeft.z - origin.z) / dir.z;
            double t6 = (lowerLeft.z + side - origin.z) / dir.z;

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

        // Input Operator Overloading
        friend istream& operator >> (istream& input, Cube& cube) {
            input >> cube.lowerLeft;
            input >> cube.side;
            input >> cube.color;
            for (int i = 0; i < 4; i++) {
                input >> cube.coEfficients[i];
            }
            input >> cube.shine;
            return input;
        }
};

double determinant(double a[3][3]) {
    double det = 0;
    for (int i = 0; i < 3; i++) {
        det += a[0][i] * (a[1][(i + 1) % 3] * a[2][(i + 2) % 3] - a[1][(i + 2) % 3] * a[2][(i + 1) % 3]);
    }
    return det;
}

class Triangle : public Object {
    public:
        Point a, b, c;

        Triangle() {
            a = Point(0, 0, 0);
            b = Point(0, 0, 0);
            c = Point(0, 0, 0);
            color = Color();
        }

        Triangle(Point a, Point b, Point c) : a(a), b(b), c(c), Object() {}

        Triangle(Point a, Point b, Point c, Color color) : a(a), b(b), c(c), Object(color, NULL, 0) {}

        Triangle(Point a, Point b, Point c, Color color, double coEfficients[4], double shine) : a(a), b(b), c(c), Object(color, coEfficients, shine) {}

        Triangle(Point a, Point b, Point c, Color color, double coEfficients[4], double shine, bitmap_image texture) : a(a), b(b), c(c), Object(color, coEfficients, shine, texture) {}

        void draw() {
            glColor3f(color.r, color.g, color.b);
            glBegin(GL_TRIANGLES);
                glVertex3f(a.x, a.y, a.z);
                glVertex3f(b.x, b.y, b.z);
                glVertex3f(c.x, c.y, c.z);
            glEnd();
        }

        double getIntersectingT(Ray ray) {
            Point origin = ray.origin;
            Point dir = ray.dir;

            double AMatrix[3][3] = {
                {a.x - b.x, a.x - c.x, dir.x},
                {a.y - b.y, a.y - c.y, dir.y},
                {a.z - b.z, a.z - c.z, dir.z}
            };

            double betaMatrix[3][3] = {
                {a.x - origin.x, a.x - c.x, dir.x},
                {a.y - origin.y, a.y - c.y, dir.y},
                {a.z - origin.z, a.z - c.z, dir.z}
            };

            double gammaMatrix[3][3] = {
                {a.x - b.x, a.x - origin.x, dir.x},
                {a.y - b.y, a.y - origin.y, dir.y},
                {a.z - b.z, a.z - origin.z, dir.z}
            };

            double tMatrix[3][3] = {
                {a.x - b.x, a.x - c.x, a.x - origin.x},
                {a.y - b.y, a.y - c.y, a.y - origin.y},
                {a.z - b.z, a.z - c.z, a.z - origin.z}
            };


            double A = determinant(AMatrix);
            double beta = determinant(betaMatrix) / A;
            double gamma = determinant(gammaMatrix) / A;
            double t = determinant(tMatrix) / A;

            if (beta >= 0 && gamma >= 0 && beta + gamma <= 1 && t > 0) {
                return t;
            } else {
                return -1;
            }
        }

        Point getNormal(Point intersectionPoint) {
            Point normal = (b - a) ^ (c - a);
            normal.normalize();
            return normal;
        }

        Color getColor(Point intersectionPoint) {
            return color;
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
        Triangle triangles[6];

        Pyramid() {
            lowerLeft = Point(0, 0, 0);
            width = height = 1;
            color = Color();

            initializeTriangles();
        }

        Pyramid(Point lowerLeft, double width, double height) : lowerLeft(lowerLeft), width(width), height(height) {
            initializeTriangles();
        }

        Pyramid(Point lowerLeft, double width, double height, Color color, double coEfficients[4], double shine) : lowerLeft(lowerLeft), width(width), height(height) {
            this->color = color;
            this->shine = shine;
            for (int i = 0; i < 4; i++) {
                this->coEfficients[i] = coEfficients[i];
            }

            initializeTriangles();
        }

        void initializeTriangles() {
            // Four triangles for the sides of the pyramid taking the lower left point as the origin
            triangles[0] = Triangle(lowerLeft, Point(lowerLeft.x + width, lowerLeft.y, lowerLeft.z), Point(lowerLeft.x + width / 2, lowerLeft.y + height, lowerLeft.z + width / 2), color, coEfficients, shine);
            triangles[1] = Triangle(lowerLeft, Point(lowerLeft.x, lowerLeft.y, lowerLeft.z + width), Point(lowerLeft.x + width / 2, lowerLeft.y + height, lowerLeft.z + width / 2), color, coEfficients, shine);
            triangles[2] = Triangle(Point(lowerLeft.x, lowerLeft.y, lowerLeft.z + width), Point(lowerLeft.x + width, lowerLeft.y, lowerLeft.z + width), Point(lowerLeft.x + width / 2, lowerLeft.y + height, lowerLeft.z + width / 2), color, coEfficients, shine);
            triangles[3] = Triangle(Point(lowerLeft.x + width, lowerLeft.y, lowerLeft.z), Point(lowerLeft.x + width, lowerLeft.y, lowerLeft.z + width), Point(lowerLeft.x + width / 2, lowerLeft.y + height, lowerLeft.z + width / 2), color, coEfficients, shine);

            // Two triangles for the base of the pyramid
            triangles[4] = Triangle(lowerLeft, Point(lowerLeft.x + width, lowerLeft.y, lowerLeft.z), Point(lowerLeft.x, lowerLeft.y, lowerLeft.z + width), color, coEfficients, shine);
            triangles[5] = Triangle(Point(lowerLeft.x + width, lowerLeft.y, lowerLeft.z + width), Point(lowerLeft.x, lowerLeft.y, lowerLeft.z + width), Point(lowerLeft.x + width, lowerLeft.y, lowerLeft.z), color, coEfficients, shine);
        }


        void draw() {
            for (int i = 0; i < 6; i++) {
                triangles[i].draw();
            }
        }

        double getIntersectingT(Ray ray) {
            Point origin = ray.origin;
            Point dir = ray.dir;

            double tmin = -1;

            Triangle face1 = Triangle(lowerLeft, Point(lowerLeft.x + width, lowerLeft.y, lowerLeft.z), Point(lowerLeft.x + width / 2, lowerLeft.y + height, lowerLeft.z + width / 2));
            Triangle face2 = Triangle(lowerLeft, Point(lowerLeft.x, lowerLeft.y, lowerLeft.z + width), Point(lowerLeft.x + width / 2, lowerLeft.y + height, lowerLeft.z + width / 2));
            Triangle face3 = Triangle(Point(lowerLeft.x, lowerLeft.y, lowerLeft.z + width), Point(lowerLeft.x + width, lowerLeft.y, lowerLeft.z + width), Point(lowerLeft.x + width / 2, lowerLeft.y + height, lowerLeft.z + width / 2));
            Triangle face4 = Triangle(Point(lowerLeft.x + width, lowerLeft.y, lowerLeft.z), Point(lowerLeft.x + width, lowerLeft.y, lowerLeft.z + width), Point(lowerLeft.x + width / 2, lowerLeft.y + height, lowerLeft.z + width / 2));

            double t1 = face1.getIntersectingT(ray);
            double t2 = face2.getIntersectingT(ray);
            double t3 = face3.getIntersectingT(ray);
            double t4 = face4.getIntersectingT(ray);

            // return minimum of t1, t2, t3, t4
            if (t1 > 0) {
                tmin = t1;
            }
            if (t2 > 0) {
                if (tmin < 0) {
                    tmin = t2;
                } else {
                    tmin = min(tmin, t2);
                }
            }
            if (t3 > 0) {
                if (tmin < 0) {
                    tmin = t3;
                } else {
                    tmin = min(tmin, t3);
                }
            }
            if (t4 > 0) {
                if (tmin < 0) {
                    tmin = t4;
                } else {
                    tmin = min(tmin, t4);
                }
            }

            // Intersection with the base
            double t5 = (lowerLeft.y - origin.y) / dir.y;

            if (t5 > 0) {
                Point intersectionPoint = origin + dir * t5;
                if (intersectionPoint.x >= lowerLeft.x && intersectionPoint.x <= lowerLeft.x + width && intersectionPoint.z >= lowerLeft.z && intersectionPoint.z <= lowerLeft.z + width) {
                    if (tmin < 0) {
                        tmin = t5;
                    } else {
                        tmin = min(tmin, t5);
                    }
                }
            }
            

            return tmin;
        }

        Color getColor(Point intersectionPoint) {
            return color;
        }

        Point getNormal(Point intersectionPoint) {
            Point normal;

            // calculate the normal on the intersection point

        }

        // Input Operator Overloading
        // Input signature
        // pyramid
        // -40.0 0.0 5.0    	lowest point co-ordinate
        // 30.0 40.0        	width height
        // 1.0 0.0 0.0      	color
        // 0.4 0.2 0.0 0.4  	ambient diffuse specular reflection coefficient
        // 1		 	shininess
        friend istream& operator >> (istream &input, Pyramid &pyramid) {
            input >> pyramid.lowerLeft.x >> pyramid.lowerLeft.y >> pyramid.lowerLeft.z;
            input >> pyramid.width >> pyramid.height;
            input >> pyramid.color.r >> pyramid.color.g >> pyramid.color.b;
            for (int i = 0; i < 4; i++) {
                input >> pyramid.coEfficients[i];
            }
            input >> pyramid.shine;

            pyramid.initializeTriangles();
            return input;
        };

        void print() {
            cout << "Pyramid" << endl;
            cout << "Lower Left : " << lowerLeft << endl;
            cout << "Width : " << width << endl;
            cout << "Height : " << height << endl;
            cout << "Color : " << color << endl;
            cout << "Co-efficients : ";
            for (int i = 0; i < 4; i++) {
                cout << coEfficients[i] << " ";
            }
            cout << endl;
            cout << "Shine : " << shine << endl;
        }
};

