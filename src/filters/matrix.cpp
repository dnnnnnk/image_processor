#include "filters/matrix.h"

MatrixFilter::MatrixFilter(const Kernel3x3& kernel) : kernel_(kernel) {
}

void MatrixFilter::Apply(Image& image) const {
    const Image source = image;
    for (int y = 0; y < image.GetHeight(); ++y) {
        for (int x = 0; x < image.GetWidth(); ++x) {
            image.SetPixel(x, y, ApplyKernel(source, x, y));
        }
    }
}

Color MatrixFilter::ApplyKernel(const Image& image, int x, int y) const {
    double red = 0.0;
    double green = 0.0;
    double blue = 0.0;

    for (int kernel_y = -1; kernel_y <= 1; ++kernel_y) {
        for (int kernel_x = -1; kernel_x <= 1; ++kernel_x) {
            const Color& color = image.AtClamped(x + kernel_x, y + kernel_y);
            const double coefficient = kernel_[static_cast<size_t>(kernel_y + 1)][static_cast<size_t>(kernel_x + 1)];
            red += color.GetRed() * coefficient;
            green += color.GetGreen() * coefficient;
            blue += color.GetBlue() * coefficient;
        }
    }

    return Color(red, green, blue);
}
