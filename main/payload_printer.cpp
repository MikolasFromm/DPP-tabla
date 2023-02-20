#include "payload_printer.hpp"
#include "time_getter.hpp"

void payload_printer::print_payload(TFT_eSPI& display, U8g2_for_TFT_eSPI& adv_font_package, payload_parser& payload, time_getter& tg, std::string& stop_nickname)
{
    if (tg.try_get_current_time())
    {
        for (size_t i = 0; i < DOWNLOAD_LIMIT; i++)
        {
            JsonObject root = payload.doc[i];
            std::string arrival_timestamp_predicted = root["arrival_timestamp"]["predicted"]; // "2023-02-11T22:39:00+01:00"
            
            if (tg.try_parse_string_timestamp(arrival_timestamp_predicted))
            {
                int time_diff = tg.get_timedelta_in_minutes();
                std::string route_short_name = root["route"]["short_name"]; // line number

                JsonObject root_delay = root["delay"];
                int delay = root_delay["minutes"];

                JsonObject root_trip = root["trip"];
                std::string root_trip_headsign = root_trip["headsign"]; // line destination

                // printing line number
                if (this->line_buffer[i] != route_short_name)
                {
                    adv_font_package.setFont(u8g2_font_helvB18_te); // bigger font for line number
                    adv_font_package.setCursor(5, ((43 * i) + 30));
                    display.fillRect(5, ((43 * i) + 5), 50, 30, TFT_BLACK);
                    adv_font_package.print(route_short_name.c_str());
                    this->line_buffer[i] = route_short_name;
                }

                // printing line destination
                if (this->line_orientation_buffer[i] != root_trip_headsign)
                {
                    adv_font_package.setFont(u8g2_font_helvB12_te); // smaller font for line orientation
                    adv_font_package.setCursor(70, ((43 * i) + 30));
                    display.fillRect(70, ((43 * i) + 12), 200, 25, TFT_BLACK);
                    adv_font_package.print(root_trip_headsign.c_str());
                    this->line_orientation_buffer[i] = root_trip_headsign;
                }

                // printing time remaining in minutes
                display.setTextSize(2);
                display.setTextDatum(BR_DATUM);
                if (time_diff < 1)
                {
                    display.setTextColor(TFT_ORANGE, TFT_BLACK);
                    display.setTextPadding(50);
                    display.drawString("<1", 290, ((43 * i) + 30), 1);
                }
                else 
                {
                    if (delay >= 1)
                    {
                        display.setTextColor(TFT_RED, TFT_BLACK);
                    }
                    if (delay < 1)
                    {
                        display.setTextColor(TFT_GREEN, TFT_BLACK);
                    }
                    display.setTextPadding(45);
                    display.drawString(std::to_string(time_diff).c_str(), 290, ((43 * i) + 30), 1);
                }

                // min footer
                display.setTextSize(1);
                display.setTextDatum(BR_DATUM);
                display.setTextPadding(25);
                display.drawString(" min", 320, ((43 * i) + 30), 1);

                // printing stop nickname
                if (this-> nickname_buffer != stop_nickname)
                {
                    this->nickname_buffer = stop_nickname;
                    display.setTextColor(TFT_WHITE);
                    display.setTextSize(2);
                    display.setTextDatum(BC_DATUM);
                    display.fillRect(0, 210, 320, 30, TFT_BLACK); // making black rectangle behind to cover old text
                    display.drawString(this->nickname_buffer.c_str(), 160, 235, 1);
                }
            }
        }
    }
}

void payload_printer::clean_buffers()
{
    for (size_t i = 0; i < DOWNLOAD_LIMIT; i++)
    {
        this->line_buffer[i] = "";
        this->line_orientation_buffer[i] = "";
        this->nickname_buffer = "";
    }
}