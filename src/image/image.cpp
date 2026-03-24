#include "image/image.h"

#include <stdexcept>

Image::Image() : width_(0), height_(0), pixels_() {
}

Image::Image(int width, int height) : width_(width), height_(height), pixels_() {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Image size must be positive");
    }
    pixels_.assign(static_cast<size_t>(width_) * static_cast<size_t>(height_), Color());
}

int Image::GetWidth() const {
    return width_;
}

int Image::GetHeight() const {
    return height_;
}

bool Image::IsEmpty() const {
    return width_ == 0 || height_ == 0;
}

Color& Image::At(int x, int y) {
    return pixels_.at(GetIndex(x, y));
}

const Color& Image::At(int x, int y) const {
    return pixels_.at(GetIndex(x, y));
}

const Color& Image::AtClamped(int x, int y) const {
    return At(ClampCoord(x, width_), ClampCoord(y, height_));
}

void Image::SetPixel(int x, int y, const Color& color) {
    At(x, y) = color;
}

int Image::ClampCoord(int value, int size) {
    if (size <= 0) {
        return 0;
    }
    if (value < 0) {
        return 0;
    }
    if (value >= size) {
        return size - 1;
    }
    return value;
}

size_t Image::GetIndex(int x, int y) const {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        throw std::out_of_range("Pixel coordinates are out of range");
    }
    return static_cast<size_t>(y) * static_cast<size_t>(width_) + static_cast<size_t>(x);
}
