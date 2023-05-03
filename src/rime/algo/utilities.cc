//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2013-01-30 GONG Chen <chen.sst@gmail.com>
//
#include <fstream>
#include <rime/algo/utilities.h>
#include <zlib.h>

namespace rime {

int CompareVersionString(const string& x, const string& y) {
  if (x.empty() && y.empty()) return 0;
  if (x.empty()) return -1;
  if (y.empty()) return 1;
  vector<string> xx, yy;
  Split(xx, x, '.');
  Split(yy, y, '.');
  size_t i = 0;
  for (; i < xx.size() && i < yy.size(); ++i) {
    int dx = atoi(xx[i].c_str());
    int dy = atoi(yy[i].c_str());
    if (dx != dy) return dx - dy;
    int c = xx[i].compare(yy[i]);
    if (c != 0) return c;
  }
  if (i < xx.size()) return 1;
  if (i < yy.size()) return -1;
  return 0;
}

ChecksumComputer::ChecksumComputer(uint32_t initial_remainder)
    : crc_(crc32(initial_remainder, nullptr, 0)) {}

void ChecksumComputer::ProcessFile(const string& file_name) {
  std::ifstream fin(file_name.c_str());
  string file_content((std::istreambuf_iterator<char>(fin)),
                           std::istreambuf_iterator<char>());
  crc_ = crc32(crc_, reinterpret_cast<const Bytef*>(file_content.data()),
               file_content.length());
}

uint32_t ChecksumComputer::Checksum() {
  return crc_;
}

}  // namespace rime
