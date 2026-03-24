#pragma once

#include "filters/base.h"

class CropFilter : public Filter {
 public:
  CropFilter(int width, int height);
  void Apply(Image& image) const override;

 private:
  int width_;
  int height_;
};
