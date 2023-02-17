#include "config_getter.hpp"

void config_getter::read_config()
{
    enum load_status {nothing, whole_stop, stop_sign, timeout};
    load_status current_status = load_status::nothing;
    std::string buffer;

    if (SD.begin(13, 15, 2, 14))
    {
        bool line_comment = false;
        ext::File new_file = SD.open("/config.txt", FILE_READ);
        if (new_file)
        {
            while(new_file.available())
            {
                char current_char = new_file.read();
                if (std::isspace(current_char) || std::iscntrl(current_char)) // when on space, save the word
                {
                    if (buffer != "" && !line_comment)
                    {
                        if ((buffer == "Sloupky:" || buffer == "Zastavky:") && current_status != load_status::nothing)
                        {
                            // ERROR!
                        }
                        if (buffer == "TimeOffset:")
                        {
                            current_status = load_status::nothing; // fallback jump to ignore the "TimeOffset" word and start reading the number
                        }
                        switch (current_status)
                        {
                            case load_status::nothing:
                                if (buffer == "Zastavky:")
                                {
                                    current_status = load_status::whole_stop;
                                }
                                else if (buffer == "Sloupky:")
                                {
                                    current_status = load_status::stop_sign;
                                }
                                else if (buffer == "TimeOffset:")
                                {
                                    current_status = load_status::timeout;
                                }
                                else
                                {
                                    // ERROR!
                                }
                                break;
                            case load_status::stop_sign:
                                this->stop_signs.push_back(buffer);
                                break;
                            case load_status::whole_stop:
                                this->stop_names.push_back(buffer);
                                break;
                            case load_status::timeout:
                                this->time_offset = std::stoi(buffer);
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
        }
        else
        {
            Serial.println("Couldnt open file");
        }
        new_file.close();
        SD.end();
    }
    else
    {
        Serial.println("COULDNT BEGIN SD");
    }
    Serial.println("ZASTAVKY: ");
    for (size_t i = 0; i < this->stop_names.size(); i++)
    {
        Serial.print(this->stop_names[i].c_str());
        Serial.print(" ; ");
    }
    Serial.println("SLOUPKY: ");
    for (size_t i = 0; i < this->stop_signs.size(); i++)
    {
        Serial.print(this->stop_signs[i].c_str());
        Serial.print(" ; ");
    }
    Serial.println();
    
    return;
}