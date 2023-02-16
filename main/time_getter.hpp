#ifndef time_getter_hpp_
#define time_getter_hpp_

#include "main_const.hpp"

#include "time.h"
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

class time_getter
{
  public:
    bool try_get_current_time();
    bool try_parse_string_timestamp(std::string& timestamp);
    const struct tm& get_current_time();
    const struct tm& get_parsed_time();
    int get_timedelta_in_minutes();
  private:
    // NTP config
    const char* ntpServer = "pool.ntp.org"; //TIME-SERVER
    const long  gmtOffset_sec = 0; //OFFSETS (7200)
    const int   daylightOffset_sec = 0; //OFFSETS

    // time values
    struct tm current_time;
    struct tm parsed_string_time;
};

#endif
