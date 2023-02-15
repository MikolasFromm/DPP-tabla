#include "time_getter.hpp"
#include "esp32-hal-time.c"

bool time_getter::try_get_current_time()
{
    configTime(this->gmtOffset_sec, this->daylightOffset_sec, this->ntpServer);
    if (!getLocalTime(&this->current_time))
        return false;
    else
        return true;
}

bool time_getter::try_parse_string_timestamp(std::string& timestamp)
{
    std::istringstream ss(timestamp);
    ss >> std::get_time(&this->parsed_string_time, "%Y-%m-%dT%H:%M:%S+01:00");
    if (ss.fail())
        return false;
    else
        return true;
}

int time_getter::get_timedelta_in_minutes()
{
    std::time_t time_now = std::mktime(&this->current_time);
    std::time_t time_parsed = std::mktime(&this->parsed_string_time);
    return int(std::difftime(time_now, time_parsed));
}