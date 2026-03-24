#pragma once

#include "filters/base.h"

class GrayscaleFilter : public Filter {
 public:
  void Apply(Image& image) const override;
};
