#pragma once

#include "filters/base.h"

class PixelateFilter : public Filter {
 public:
  explicit PixelateFilter(int block_size);
  void Apply(Image& image) const override;

 private:
  int block_size_;
};
