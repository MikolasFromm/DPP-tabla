#ifndef config_getter_hpp_
#define config_getter_hpp_

#include <mySD.h>
#include <SPI.h>
#include <vector>
#include <string>

class config_getter
{
  public:
  void read_config();
  private:
  std::vector<std::string> stop_names;
  std::vector<std::string> stop_signs;
  int time_offset;
};

#endif
