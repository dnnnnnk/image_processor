#include "filters/edge_detection.h"

#include "filters/grayscale.h"
#include <stdexcept>

EdgeDetectionFilter::EdgeDetectionFilter(double threshold)
    : MatrixFilter({{{0.0, -1.0, 0.0}, {-1.0, 4.0, -1.0}, {0.0, -1.0, 0.0}}}), threshold_(threshold) {
    if (threshold_ < 0.0 || threshold_ > 1.0) {
        throw std::invalid_argument("edge threshold must be in [0, 1]");
    }
}

void EdgeDetectionFilter::Apply(Image& image) const {
    GrayscaleFilter grayscale_filter;
    grayscale_filter.Apply(image);
    MatrixFilter::Apply(image);

    for (int y = 0; y < image.GetHeight(); ++y) {
        for (int x = 0; x < image.GetWidth(); ++x) {
            const double value = image.At(x, y).GetRed();
            const double black_or_white = value > threshold_ ? 1.0 : 0.0;
            image.SetPixel(x, y, Color(black_or_white, black_or_white, black_or_white));
        }
    }
}
