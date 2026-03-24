#include "image/color.h"

Color::Color() : red_(0.0), green_(0.0), blue_(0.0) {
}

Color::Color(double red, double green, double blue) : red_(Clamp(red)), green_(Clamp(green)), blue_(Clamp(blue)) {
}

double Color::GetRed() const {
    return red_;
}

double Color::GetGreen() const {
    return green_;
}

double Color::GetBlue() const {
    return blue_;
}

void Color::SetRed(double red) {
    red_ = Clamp(red);
}

void Color::SetGreen(double green) {
    green_ = Clamp(green);
}

void Color::SetBlue(double blue) {
    blue_ = Clamp(blue);
}

double Color::Clamp(double value) {
    if (value < 0.0) {
        return 0.0;
    }
    if (value > 1.0) {
        return 1.0;
    }
    return value;
}
