// RUN: %zap_compare_object CXXFLAGS="-I %S -include include-command-line.h"
// Bug 550
// Bug 551
#ifndef INCLUDE_COMMAND_LINE
#error include-command-line.h was not included
#endif
