#include <iomanip>
#include "Color.h"

Color::Color() {
    this->r = 0;
    this->g = 0;
    this->b = 0;
}

Color::Color(double r, double g, double b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

Color::Color(const Color &other)
{
    this->r = other.r;
    this->g = other.g;
    this->b = other.b;
}

Color Color::operator+(const Color & rhs) const {
    Color c;
    c.r = r + rhs.r;
    c.g = g + rhs.g;
    c.b = b + rhs.b;
    return c;
}

Color Color::operator-(const Color & rhs) const {
    Color c;
    c.r = r - rhs.r;
    c.g = g - rhs.g;
    c.b = b - rhs.b;
    return c;
}

Color Color::operator*(double num) const {
    Color c;
    c.r = r*num;
    c.g = g*num;
    c.b = b*num;
    return c;
}

Color Color::operator/(double num) const {
    Color c;
    c.r = r/num;
    c.g = g/num;
    c.b = b/num;
    return c;
}

Color& Color::operator-=(const Color &other) {
    r -= other.r;
    g -= other.g;
    b -= other.b;
    return *this;
}


Color& Color::operator+=(const Color &other) {
    r += other.r;
    g += other.g;
    b += other.b;
    return *this;
}

Color& Color::operator*=(double scalar) {
    r *= scalar;
    g *= scalar;
    b *= scalar;
    return *this;

}



Color Color::round() const {
    Color c;
    c.r = (int)(r + 0.5);
    c.g = (int)(g + 0.5);
    c.b = (int)(b + 0.5);
    return c;
}





std::ostream &operator<<(std::ostream &os, const Color &c)
{
    os << std::fixed << std::setprecision(0) << "rgb(" << c.r << ", " << c.g << ", " << c.b << ")";
    return os;
}
