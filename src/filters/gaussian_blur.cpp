#include "filters/gaussian_blur.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

GaussianBlurFilter::GaussianBlurFilter(double sigma) : kernel_() {
    if (sigma <= 0.0 || !std::isfinite(sigma)) {
        throw std::invalid_argument("blur sigma must be positive");
    }
    kernel_ = BuildKernel(sigma);
}

void GaussianBlurFilter::Apply(Image& image) const {
    const Image horizontal = BlurHorizontal(image);
    image = BlurVertical(horizontal);
}

std::vector<double> GaussianBlurFilter::BuildKernel(double sigma) {
    const int radius = std::max(1, static_cast<int>(std::ceil(3.0 * sigma)));
    std::vector<double> kernel(static_cast<size_t>(2 * radius + 1), 0.0);
    double sum = 0.0;

    for (int i = -radius; i <= radius; ++i) {
        const double value = std::exp(-(static_cast<double>(i * i)) / (2.0 * sigma * sigma));
        kernel[static_cast<size_t>(i + radius)] = value;
        sum += value;
    }

    for (double& value : kernel) {
        value /= sum;
    }

    return kernel;
}

Image GaussianBlurFilter::BlurHorizontal(const Image& image) const {
    Image result(image.GetWidth(), image.GetHeight());
    const int radius = static_cast<int>(kernel_.size() / 2);

    for (int y = 0; y < image.GetHeight(); ++y) {
        for (int x = 0; x < image.GetWidth(); ++x) {
            double red = 0.0;
            double green = 0.0;
            double blue = 0.0;

            for (int dx = -radius; dx <= radius; ++dx) {
                const double weight = kernel_[static_cast<size_t>(dx + radius)];
                const Color& color = image.AtClamped(x + dx, y);
                red += color.GetRed() * weight;
                green += color.GetGreen() * weight;
                blue += color.GetBlue() * weight;
            }

            result.SetPixel(x, y, Color(red, green, blue));
        }
    }

    return result;
}

Image GaussianBlurFilter::BlurVertical(const Image& image) const {
    Image result(image.GetWidth(), image.GetHeight());
    const int radius = static_cast<int>(kernel_.size() / 2);

    for (int y = 0; y < image.GetHeight(); ++y) {
        for (int x = 0; x < image.GetWidth(); ++x) {
            double red = 0.0;
            double green = 0.0;
            double blue = 0.0;

            for (int dy = -radius; dy <= radius; ++dy) {
                const double weight = kernel_[static_cast<size_t>(dy + radius)];
                const Color& color = image.AtClamped(x, y + dy);
                red += color.GetRed() * weight;
                green += color.GetGreen() * weight;
                blue += color.GetBlue() * weight;
            }

            result.SetPixel(x, y, Color(red, green, blue));
        }
    }

    return result;
}
