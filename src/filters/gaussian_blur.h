#pragma once

#include "filters/base.h"
#include <vector>

class GaussianBlurFilter : public Filter {
 public:
  explicit GaussianBlurFilter(double sigma);
  void Apply(Image& image) const override;

 private:
  static std::vector<double> BuildKernel(double sigma);
  Image BlurHorizontal(const Image& image) const;
  Image BlurVertical(const Image& image) const;

  std::vector<double> kernel_;
};
