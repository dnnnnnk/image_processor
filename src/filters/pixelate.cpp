#include "filters/pixelate.h"

#include <algorithm>
#include <stdexcept>

PixelateFilter::PixelateFilter(int block_size) : block_size_(block_size) {
    if (block_size_ <= 0) {
        throw std::invalid_argument("pixelate block size must be positive");
    }
}

void PixelateFilter::Apply(Image& image) const {
    const Image source = image;

    for (int y0 = 0; y0 < image.GetHeight(); y0 += block_size_) {
        for (int x0 = 0; x0 < image.GetWidth(); x0 += block_size_) {
            const int y1 = std::min(y0 + block_size_, image.GetHeight());
            const int x1 = std::min(x0 + block_size_, image.GetWidth());

            double red = 0.0;
            double green = 0.0;
            double blue = 0.0;
            int count = 0;

            for (int y = y0; y < y1; ++y) {
                for (int x = x0; x < x1; ++x) {
                    const Color& color = source.At(x, y);
                    red += color.GetRed();
                    green += color.GetGreen();
                    blue += color.GetBlue();
                    ++count;
                }
            }

            const Color average(red / static_cast<double>(count), green / static_cast<double>(count),
                                blue / static_cast<double>(count));

            for (int y = y0; y < y1; ++y) {
                for (int x = x0; x < x1; ++x) {
                    image.SetPixel(x, y, average);
                }
            }
        }
    }
}
