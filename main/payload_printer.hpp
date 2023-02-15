#ifndef paylaod_printer_hpp_
#define payload_printer_hpp_

#include <time.h>
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>
#include "payload_parser.hpp"
#include "time_getter.hpp"

class payload_printer
{
    public:
    void print_payload(TFT_eSPI& display, U8g2_for_TFT_eSPI& adv_font_package, payload_parser& payload, time_getter& tg);
    private:
    std::string line_buffer[DOWNLOAD_LIMIT];
    std::string line_orientation_buffer[DOWNLOAD_LIMIT];
};

#endif
