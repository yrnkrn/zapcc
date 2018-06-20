// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef CEEMPLE_TEXTFILE_H
#define CEEMPLE_TEXTFILE_H

#include <string>

// Return true on success, false on failure.
bool readRealTextFile(const std::string &FileName, std::string &Text);
bool writeRealTextFile(const std::string &FileName, const std::string &Text,
                       bool Append = false);

#endif // CEEMPLE_TEXTFILE_H
