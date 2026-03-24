#pragma once

#include "image/color.h"
#include <cstddef>
#include <vector>

class Image {
 public:
  Image();
  Image(int width, int height);
  int GetWidth() const;
  int GetHeight() const;
  bool IsEmpty() const;
  Color& At(int x, int y);
  const Color& At(int x, int y) const;
  const Color& AtClamped(int x, int y) const;
  void SetPixel(int x, int y, const Color& color);

 private:
  static int ClampCoord(int value, int size);
  size_t GetIndex(int x, int y) const;

  int width_;
  int height_;
  std::vector<Color> pixels_;
};
