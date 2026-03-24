#pragma once

#include "filters/base.h"

class NegativeFilter : public Filter {
 public:
  void Apply(Image& image) const override;
};
