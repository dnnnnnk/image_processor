#include "filters/crop.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

CropFilter::CropFilter(int width, int height) : width_(width), height_(height) {
    if (width_ <= 0 || height_ <= 0) {
        throw std::invalid_argument("crop parameters must be positive");
    }
}

void CropFilter::Apply(Image& image) const {
    if (image.IsEmpty()) {
        return;
    }

    const int new_width = std::min(width_, image.GetWidth());
    const int new_height = std::min(height_, image.GetHeight());
    Image cropped(new_width, new_height);

    for (int y = 0; y < new_height; ++y) {
        for (int x = 0; x < new_width; ++x) {
            cropped.SetPixel(x, y, image.At(x, y));
        }
    }

    image = std::move(cropped);
}
