#pragma once

#include "filters/matrix.h"

class EdgeDetectionFilter : public MatrixFilter {
 public:
  explicit EdgeDetectionFilter(double threshold);
  void Apply(Image& image) const override;

 private:
  double threshold_;
};
