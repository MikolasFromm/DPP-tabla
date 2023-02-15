#ifndef time_getter_hpp_
#define time_getter_hpp_

#include "time.h"
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

class time_getter
{
  public:
    bool try_get_current_time();
    struct tm current_time;
    bool try_parse_string_timestamp(std::string& timestamp);
    struct tm parsed_string_time;
    int get_timedelta_in_minutes();
  private:
    const char* ntpServer = "pool.ntp.org"; //TIME-SERVER
    const long  gmtOffset_sec = 0; //OFFSETS (7200)
    const int   daylightOffset_sec = 0; //OFFSETS
};

#endif
