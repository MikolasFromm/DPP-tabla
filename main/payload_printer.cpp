#include "payload_printer.hpp"
#include "time_getter.hpp"
#include "main_const.hpp"

std::string payload_printer::cut_string(std::string& origin)
{
    if (origin.length() > DISP_TEXT_MAX_LEN)
    {
        std::vector<std::string> name;
        name.push_back("");
        // cutting into vector
        size_t index = 0;
        for (size_t i = 0; i < origin.length(); i++)
        {
            if (origin[i] == ' ')
            {
                index++;
                name.push_back("");
            }
            else
                name[index] += origin[i];
        }
        // shortening first positions
        index = 0;
        if (name.size() > 1)
        {
            if (name.size() == 2)
            {
                name[0] = name[0].substr(0, 4) + ".";
            }
            else
            {
                while (index < name.size() - 1) // last word must be left
                {
                    // when the first char is > 'z', it is very likely a diacritics, therefore add even the second letter.
                    if (name[index][0] > 'z')
                    {
                        name[index] = name[index].substr(0, 2);
                        name[index] += ".";
                    }
                    // else show just the first letter
                    else
                    {
                        name[index] = name[index][0];
                        name[index] += ".";
                    }
                    index++;
                }
            }
            // getting words back together
            std::string res = "";
            size_t name_size = name.size();
            for (size_t i = 0; i < name_size; i++)
            {
                res += name[i];
                if (i != name_size - 1)
                    res += " ";
            }
            return res;
        }
        else
        {
            return name[0];
        }
    }
    return origin;
}

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
                    size_t offset = 5;
                    offset += adv_font_package.getUTF8Width("1")*(3 - route_short_name.length())*2;
                    Serial.println(offset); 
                    this->line_buffer[i] = route_short_name;
                    adv_font_package.setFont(u8g2_font_helvB18_te); // bigger font for line numbers
                    adv_font_package.setCursor(offset, ((43 * i) + 30));
                    display.fillRect(5, ((43 * i) + 5), 50, 30, TFT_BLACK);
                    adv_font_package.print(route_short_name.c_str());
                }

                // printing line destination
                if (this->line_orientation_buffer[i] != root_trip_headsign)
                {
                    this->line_orientation_buffer[i] = root_trip_headsign;
                    adv_font_package.setFont(u8g2_font_helvB12_te); // smaller font for line orientation
                    adv_font_package.setCursor(70, ((43 * i) + 30));
                    display.fillRect(70, ((43 * i) + 12), 200, 25, TFT_BLACK);
                    adv_font_package.print(cut_string(root_trip_headsign).c_str());
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
            else
            {
              Serial.print("Could not parse given timestamp: "); Serial.println(arrival_timestamp_predicted.c_str());
            }
        }
    }
    else
    {
      Serial.println("Could not obtain current time");
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
