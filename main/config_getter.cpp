#include "config_getter.hpp"

void config_getter::read_config(TFT_eSPI& tft)
{
    enum load_status {waiting, stop, time, nick, ssid, pass, api};
    load_status current_status = load_status::waiting;
    
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(3);
    while(!SD.begin(13, 15, 2, 14))
    {
        // IDLE to connect SD card
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.drawString("INSERT SD CARD", 160, 60, 1);
    }
    // success notification
    tft.fillScreen(TFT_GREEN);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("Succesfully", 160, 60, 1);
    tft.drawString(" loaded SD ", 160, 90, 1);
    delay(2000);
    tft.fillScreen(TFT_BLACK);
    
    ext::File new_file = SD.open("/config.txt", F_READ);
    if (new_file)
    {
        std::string buffer;
        bool line_comment = false;
        while(new_file.available())
        {
            char current_char = new_file.read();
            if (std::isspace(current_char) || std::iscntrl(current_char)) // when on space, save the word
            {
                if (buffer != "" && !line_comment)
                {
                    if (current_status != load_status::waiting)
                            buffer = buffer.substr(1, buffer.size()-2); // removing  '"' before and after word
                    switch (current_status)
                    {
                        case load_status::waiting:
                            if (buffer == "Z:")
                            {
                                current_status = load_status::stop;
                            }
                            else if (buffer == "N:")
                            {
                                current_status = load_status::nick;
                            }
                            else if (buffer == "T:")
                            {
                                current_status = load_status::time;
                            }
                            else if (buffer == "W:")
                            {
                                current_status = load_status::ssid;
                            }
                            else if (buffer == "P:")
                            {
                                current_status = load_status::pass;
                            }
                            else if (buffer == "A:")
                            {
                                current_status = load_status::api;
                            }
                            else
                            {
                                // ERROR! WRONG DATA FORMAT
                            }
                            break;
                        case load_status::stop:
                        {
                            this->load_default_values();
                            // pushing back the new stop
                            this->stop_names.push_back(buffer);
                            current_status = load_status::waiting;
                            break;
                        }
                        case load_status::nick:
                            this->stop_nicknames.push_back(buffer);
                            current_status = load_status::waiting;
                            break;
                        case load_status::time:
                        {
                            int temp_walktime = 0; // default value
                            try
                            {
                                temp_walktime = std::stoi(buffer);
                            }
                            catch(const std::invalid_argument)
                            {
                                Serial.print("Could not parse given time: ");
                                Serial.println(buffer.c_str());
                            }
                            catch(const std::out_of_range)
                            {
                                Serial.print("Could not parse given time - OutOfRange: ");
                                Serial.println(buffer.c_str());
                            }

                            if (temp_walktime <= 0)
                                this->walktime_to_stop.push_back(temp_walktime); // used as "minutesBefore=-3"
                            else
                                this->walktime_to_stop.push_back(-1*temp_walktime); // when positive, turn to negative

                            current_status = load_status::waiting;
                            break;
                        }
                        case load_status::ssid:
                            this->ssid = buffer;
                            current_status = load_status::waiting;
                            break;
                        case load_status::pass:
                            this->wifi_pass = buffer;
                            current_status = load_status::waiting;
                            break;
                        case load_status::api:
                            this->api_key = buffer;
                            current_status = load_status::waiting;
                            break;
                        }
                    buffer = "";
                }
            }
            else // otherwise save the letter to buffer
            {
                if (current_char == '/' || current_char == '#') // ignoring line comments
                    line_comment = true;
                if (!line_comment) // saving to buffer only when not in commented line
                    buffer += current_char;
            }
            if (current_char == '\n') // when on new_line char
                line_comment = false;
        }
        this->load_default_values();
    }
    else
    {
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.setTextSize(3);
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.drawString("CONFIG NOT FOUND!", 160, 60, 1);
        while (true)
        {
            // RABBIT HOLE
        }
    }
    new_file.close();
    SD.end();
    return;
}

void config_getter::load_default_values()
{
    // if nickname or time to stop not given, pushing back default
    std::size_t count_of_stops = this->stop_names.size();
    if (this->walktime_to_stop.size() < count_of_stops)
    {
        this->walktime_to_stop.push_back(0);
    }
    if (this->stop_nicknames.size() < count_of_stops)
    {
        std::string unknown_name = "-----";
        this->stop_nicknames.push_back(unknown_name);
    }
}