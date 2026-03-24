#include "filters/negative.h"

void NegativeFilter::Apply(Image& image) const {
    for (int y = 0; y < image.GetHeight(); ++y) {
        for (int x = 0; x < image.GetWidth(); ++x) {
            const Color& color = image.At(x, y);
            image.SetPixel(x, y, Color(1.0 - color.GetRed(), 1.0 - color.GetGreen(), 1.0 - color.GetBlue()));
        }
    }
}
