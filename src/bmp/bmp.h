#pragma once

#include "image/image.h"
#include <string>

namespace bmp {
class BmpReader {
public:
  static Image Read(const std::string& path);
};

class BmpWriter {
public:
  static void Write(const std::string& path, const Image& image);
};
}
