// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include <fstream>
#include <string>
#include <vector>

bool readRealTextFile(const std::string &FileName, std::string &Text) {
  std::ifstream ifs(FileName, std::ios::in | std::ios::binary | std::ios::ate);
  if (!ifs)
    return false;
  std::ifstream::pos_type fileSize = ifs.tellg();
  ifs.seekg(0, std::ios::beg);
  unsigned uFileSize = (unsigned)fileSize;
  std::vector<char> bytes(uFileSize);
  ifs.read(&bytes[0], fileSize);
  Text.assign(&bytes[0], uFileSize);
  ifs.close();
  return true;
} // readRealTextFile

bool writeRealTextFile(const std::string &FileName, const std::string &Text,
                       bool Append) {
  std::ios_base::openmode Mode = std::ios::binary;
  if (Append)
    Mode |= std::ios::app;
  std::ofstream TextFile(FileName, Mode);
  if (!TextFile)
    return false;
  TextFile << Text;
  TextFile.close();
  return true;
} // writeRealTextFile
