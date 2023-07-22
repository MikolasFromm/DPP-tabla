# ESP32 - DPP Departure board

<p align="center">
  <img src=https://miko.fromm.one/public-pocket/fotky/DPP_TABLA_PIC.jpg alt= “” width=50%>
</p>

## Introduction

Prague departure board running on an ESP32 microcontroller together with TFT ILI9341 display. Developed as a semester project for a C++ curse.

## Data source

Current solution uses a LIVE data bank from [Golemio API](https://api.golemio.cz/v2/pid/docs/openapi/). Specifically, it uses the endpoint [https://api.golemio.cz/v2/pid/departureboards](https://api.golemio.cz/v2/pid/departureboards), which writes down top _x_ closest departures from the given stop / stop-stand in a JSON format.

It is possible to watch departures in all direction of a stop, or it is possible to focus only on a chosen direction or even a single stand of a given stop. (Usually trams and buses have separated stands.)

It is required to generate a personal API token key for your own usage on the given address: https://api.golemio.cz/api-keys/auth/sign-in

## Usage

This particular hardware configuration contains three buttons, which are used for the following tasks:
- Changing the displayed stop, choosing the next from the configuration,
- DeepSleep aka turning off the display,
- WakeFromSleep aka turning on the display.

Since this particular board is not capable of turning off the display backlit separately, it is overriden by entering DeepSleep mode, which is almost perfect, except for the fact that the board is required to reboot after the sleep because of a WiFi reconnection. This all is done automatically after waking up.

### CONFIG
In order to succesfully print any departures on this departure board, a config file must be correctly filled with a required data. The config file has a following structure:

```
## API
A: "your-api-key"

## WIFI
W: "SSID"
P: "PSSWD"

## STOP 1 with nickname and walking time
Z: "ids=U236Z1P&ids=U236Z4P"
N: "Slanska"
T: "-2"

## STOP 2 with nickname only
Z: "names=Malostransk%C3%A9%20n%C3%A1m%C4%9Bst%C3%AD"
N: "Mal.nam."

## STOP 3 without any additional info.
Z: "names=Malostransk%C3%A1"

```
    A: placeholder for your API key
    W: placeholder for your WiFi SSID (name)
    P: placeholder for your Wifi PASSWD

    Z: placeholder for stop-name argument
    N: placeholder for stop nickname argument
    T: placeholder for time to walk to the stop

Note that _"walk to the stop"_ supports both negative and positive values - both represented as a minimal time to reach the stop. If a positive value is given, it is transformed into negative values, because it uses _"timeBefore: -x "_ request.

Also note that you can combine multiple values by separating the request with "&". It is only required to use **_names=_** or **_ids=_** exclusively and not together.

If you define a nickname under the stop request definition, it will be printed at the bottom of the display to easily distinguish between the stops. 

The easiest way to get the correct stop request is to form it on [this address](https://api.golemio.cz/v2/pid/docs/openapi/#/%F0%9F%9A%8F%20PID%20Departure%20Boards/get_pid_departureboards).

The config file must be called **config.txt**, must follow the format defined in the section above and must be placed in the root of the microSD card which will be inserted.

## Project resources
Project is depending on the following Arduino libs.:

- https://github.com/Bodmer/TFT_eSPI
- https://github.com/Bodmer/U8g2_for_TFT_eSPI
- https://arduinojson.org/

Since the SSL communication combined with a parsing of a JSON documents is not a memory-free operation, it is welcomed that the board comes with 4MB of PSRAM, without which the board crashes frequently. The ArduinoJson therefore automatically allocates its memory pool into the PSRAM for each parsing request instead of the internal memory. This way the ESP32 has enough memory to survive.

## Code description

### buttons.hpp
Simple library to simplify checking if a button is pressed or not. ALso dealing with mechanical debounce with a fixed timeout before each "click".

Function `check_it()` callable on each button then simply returns `Bool` of the current button status.

### config_getter.cpp/hpp
Obtaining the configuration setup from the given `config.txt` file via microSD card.
It is a simple loading automata, where each data line must start with an existing placeholder. After a placeholder, anything behind the leading space will be considered as a valid input text.
The loading is obtained by calling function `void config_getter::read_config`.

### main_const.hpp
Consisting of four static settings of the board, namely:

- max num. of rows
- max len. of name
- timezone
- content/type

### payload_parser.cpp/hpp
Main function of the solution, which downloads and parses the JSON response.

Any call must be started with `void payload_parser::start_http_client()` and ended with `void payload_parser::end_http_client()`. Apart from that, after loading the data with `void config_getter::read_config`, it is required to call `int payload_parser::input_data_check` which not only checks if `API key` and `StopName` is given, but also creates the static GET request URL.

To obtain the parsed response, `int payload_parser::deserialize_document()` will do the job including saving the response to the external PSRAM memory to keep the device running without random shutdowns.

### payload_printer.cpp/hpp
Function to print all departures downloaded from the API.
It is expected that the `void payload_printer::print_payload` is called after obtaining the response from the API server. Therefore there is the following workaround in `main.ino`:
```
void print_payload()
{
  if (PayloadParser.deserialize_document() == 200)
  {
    PayloadPrinter.print_payload(tft, u8f, PayloadParser, TimeGetter, ConfigGetter.get_current_stop_nickname());
  }
}
```
Printer has some static dependencies from `main_const.hpp`, such as `DOWNLOAD_LIMIT` and `DISP_TEXT_MAX_LEN`. Since there is not enough space for more than 5 departures, the download is limited to save memory space. Also, depending on the given font, long FINAL-STOP names must not fit inside the space of the display. Therefore when name is longer than `DISP_TEXT_MAX_LEN`, `std::string payload_printer::cut_string(std::string& origin)` will cut the name like this:

- Ústřední dílny DP -> Ú. d. DP

leaving the last word untouched.

### time_getter.cpp/hpp
Library to work with time. Has three functions:

- obtain current time: `bool time_getter::try_get_current_time()`
- parse timestamp from API server: `bool time_getter::try_parse_string_timestamp(std::string& timestamp)`
- get delta of two times: `int time_getter::get_timedelta_in_minutes()`

The last _delta_ function is used to show minutes left to the given departure.