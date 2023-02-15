#include "time_getter.hpp"
#include "esp32-hal-time.c"

bool time_getter::try_get_current_time()
{
    configTime(this->gmtOffset_sec, this->daylightOffset_sec, this->ntpServer, nullptr, nullptr);
    if (!getLocalTime(&this->current_time, 5000U))
        return false;
    else
        return true;
}

bool time_getter::try_parse_string_timestamp(std::string& timestamp)
{
    std::istringstream ss(timestamp);
    ss >> std::get_time(&this->parsed_string_time, "%Y-%m-%dT%H:%M:%S");
    if (ss.fail())
        return false;
    else
        return true;
}

const struct tm& time_getter::get_current_time()
{
    return this->current_time;
}

const struct tm& time_getter::get_parsed_time()
{
    return this->parsed_string_time;
}

int time_getter::get_timedelta_in_minutes()
{
    std::time_t time_now = std::mktime(&this->current_time);
    std::time_t time_parsed = std::mktime(&this->parsed_string_time);
    return int(std::difftime(time_parsed, time_now)/60);
}