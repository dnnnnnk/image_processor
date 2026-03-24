#include "filters/grayscale.h"

void GrayscaleFilter::Apply(Image& image) const {
    for (int y = 0; y < image.GetHeight(); ++y) {
        for (int x = 0; x < image.GetWidth(); ++x) {
            const Color& color = image.At(x, y);
            const double gray = 0.299 * color.GetRed() + 0.587 * color.GetGreen() + 0.114 * color.GetBlue();
            image.SetPixel(x, y, Color(gray, gray, gray));
        }
    }
}
