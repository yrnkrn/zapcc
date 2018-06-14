// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef CEEMPLE_TEXTFILE_H
#define CEEMPLE_TEXTFILE_H

#include <string>

// Return true on success, false on failure.
bool readRealTextFile(const std::string &FileName, std::string &Text);
bool writeRealTextFile(const std::string &FileName, const std::string &Text,
                       bool Append = false);

#endif // CEEMPLE_TEXTFILE_H
