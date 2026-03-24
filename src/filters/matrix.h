#pragma once

#include "filters/base.h"
#include <array>

class MatrixFilter : public Filter {
 public:
  using Kernel3x3 = std::array<std::array<double, 3>, 3>;

  explicit MatrixFilter(const Kernel3x3& kernel);
  void Apply(Image& image) const override;

 protected:
  Color ApplyKernel(const Image& image, int x, int y) const;

 private:
  Kernel3x3 kernel_;
};
