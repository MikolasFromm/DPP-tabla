#include "payload_printer.hpp"
#include "time_getter.hpp"

void payload_printer::print_payload(TFT_eSPI& display, U8g2_for_TFT_eSPI& adv_font_package, payload_parser& payload, time_getter& tg)
{
    if (tg.try_get_current_time())
    {
        for (size_t i = 0; i < payload.limit; i++)
        {
            JsonObject root = payload.doc[i];
            std::string arrival_timestamp_scheduled = root["arrival_timestamp"]["scheduled"]; // "2023-02-11T22:39:00+01:00"
            
            if (tg.try_parse_string_timestamp(arrival_timestamp_scheduled))
            {
                int time_diff = tg.get_timedelta_in_minutes();
                std::string route_short_name = root["route"]["short_name"]; // line number

                JsonObject root_delay = root["delay"];
                int delay = root_delay["minutes"];

                JsonObject root_trip = root["trip"];
                std::string root_trip_headsign = root_trip["headsign"]; // line destination
                
                Serial.println(route_short_name.c_str());
                Serial.println(root_trip_headsign.c_str());
                Serial.println(arrival_timestamp_scheduled.c_str());
                Serial.print("Diff in minutes: ");
                Serial.println(time_diff);
            }
        }
    }
}