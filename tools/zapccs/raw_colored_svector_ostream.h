// Copyright (c) 2016 Ceemple Software Ltd. All rights Reserved.
#ifndef ZAPCC_COLOREDSVECTOROSTREAM_H
#define ZAPCC_COLOREDSVECTOROSTREAM_H
#include "llvm/Support/raw_ostream.h"

namespace llvm {

class raw_colored_svector_ostream : public raw_svector_ostream {
public:
  explicit raw_colored_svector_ostream(SmallVectorImpl<char> &O)
      : raw_svector_ostream(O) {}
  raw_ostream &changeColor(enum Colors colors, bool bold = false,
                           bool bg = false) override;
  raw_ostream &resetColor() override;
  raw_ostream &reverseColor() override;
  bool is_displayed() const override;
  bool has_colors() const override;
};
}

#endif
