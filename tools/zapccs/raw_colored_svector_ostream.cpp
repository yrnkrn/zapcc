// Copyright (c) 2016 Ceemple Software Ltd. All rights Reserved.
#include "raw_colored_svector_ostream.h"

#include "llvm/Support/Process.h"

using namespace llvm;

raw_ostream &raw_colored_svector_ostream::changeColor(enum Colors colors,
                                                      bool bold, bool bg) {
  const char *colorcode = (colors == SAVEDCOLOR)
                              ? sys::Process::OutputBold(bg)
                              : sys::Process::OutputColor(colors, bold, bg);
  if (colorcode)
    write(colorcode, strlen(colorcode));
  return *this;
}

raw_ostream &raw_colored_svector_ostream::resetColor() {
  const char *colorcode = sys::Process::ResetColor();
  if (colorcode)
    write(colorcode, strlen(colorcode));
  return *this;
}

raw_ostream &raw_colored_svector_ostream::reverseColor() {
  const char *colorcode = sys::Process::OutputReverse();
  if (colorcode)
    write(colorcode, strlen(colorcode));
  return *this;
}

bool raw_colored_svector_ostream::is_displayed() const { return true; }

bool raw_colored_svector_ostream::has_colors() const { return true; }
