#ifndef time_getter_hpp_
#define time_getter_hpp_

class time_getter
{
  public:
  private:
    const char* ntpServer = "pool.ntp.org"; //TIME-SERVER
    const long  gmtOffset_sec = 7200; //OFFSETS
    const int   daylightOffset_sec = 0; //OFFSETS
};

#endif
