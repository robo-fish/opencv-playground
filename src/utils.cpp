//
//  utils.cpp
//  LineDetector
//
//  Created by Kai Oezer on 28.03.19.
//

#include "utils.hpp"

namespace robofish {

class NullBuffer : public std::streambuf
{
public:
  int overflow(int c) override { return c; }
};

std::ostream & debugOut()
{
#if DEBUG
  return std::cout;
#else
  static NullBuffer null_buffer;
  static std::ostream null_stream(&null_buffer);
  return null_stream;
#endif
}

} // namespace robofish
