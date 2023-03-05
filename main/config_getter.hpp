#ifndef config_getter_hpp_
#define config_getter_hpp_

#include <mySD.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <vector>
#include <string>

class config_getter
{
  public:
  void read_config(TFT_eSPI& tft);
  void increment_stop_index() { this->stop_index = (this->stop_index+stop_names.size()+1)%stop_names.size();}
  void decrement_stop_index() { this->stop_index = (this->stop_index+stop_names.size()-1)%stop_names.size();}
  int get_current_stop_index() { return this->stop_index;}
  std::string& get_current_stop() { return this->stop_names[this->stop_index];}
  std::string& get_current_stop_nickname() { return this->stop_nicknames[this->stop_index];}
  int get_current_stop_walktime() { return this->walktime_to_stop[this->stop_index];}
  std::string& get_ssid() { return this->ssid;}
  std::string& get_wifi_pass() { return this->wifi_pass;}
  std::string& get_api_key() { return this->api_key;}


  private:
  int stop_index = 0;
  std::vector<std::string> stop_names;
  std::vector<int> walktime_to_stop;
  std::vector<std::string> stop_nicknames;
  std::string ssid;
  std::string wifi_pass;
  std::string api_key;
};

#endif
