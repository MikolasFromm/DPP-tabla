// debug definition
#define DEBUG

// global libs
#include <utility>
#include <WiFi.h>
#include <time.h>
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>
#include <string>

// local implementations
#include "config_getter.hpp"
#include "payload_parser.hpp"
#include "time_getter.hpp"

// general setup
TFT_eSPI tft = TFT_eSPI();
U8g2_for_TFT_eSPI u8f;

unsigned long Time = 0;

size_t width = 320; //RESOLUTION
size_t height = 240; //RESOLUTION

const char* SSID = WIFI_NAME; //specified in setup.h
const char* PASS = WIFI_PASS; //specified in setup.h

config_getter ConfigGetter;
payload_parser PayloadParser;
time_getter TimeGetter;

// TaskHandle_t button_task;
// TaskHandle_t data_task;

unsigned long last_time;

void setup()
{
  Serial.begin(115200);
  tft_setup();
  wifi_setup();
  input_check();
}

void loop()
{
  print_payload();
}

void tft_setup()
{
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  u8f.begin(tft);
  u8f.setFontMode(1);
  u8f.setFontDirection(0);
  u8f.setForegroundColor(TFT_WHITE);
}

void wifi_setup()
{
  ///<summary>Setup function to connect to given WiFi. Requires Serial and TFT already set-up</summary>
  WiFi.begin(SSID, PASS);

  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
  tft.drawString("Connecting...", 160, 60, 1);
  log_d("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  log_d("Connected to WiFi");

  // Succesful connection notification
  tft.setTextPadding(250);
  tft.fillScreen(TFT_GREEN);
  tft.drawString("Succesfully", 160, 60, 1);
  tft.drawString("connected!", 160, 90, 1);
  tft.setTextSize(3);
  delay(2000);
  tft.fillScreen(TFT_BLACK);

}

void input_check()
{
  switch(PayloadParser.input_data_check())
  // default -1 return value will be silently ignored, error values will be printed
  {
    case 10:
    {
      u8f.setFont(u8g2_font_helvB12_te);
      u8f.setCursor(60, 130);
      u8f.print("Zastávka nebyla určena!");
      delay(15000);
    }
    case 11:
    {
      u8f.setFont(u8g2_font_helvB12_te);
      u8f.setCursor(60, 130);
      u8f.print("Ids i jméno určeno!");
      delay(15000);
    }
    case 12:
    {
      u8f.setFont(u8g2_font_helvB12_te);
      u8f.setCursor(60, 130);
      u8f.print("Nebylo vloženo API!");
      delay(15000);
    }
  }
}

void print_payload()
{
  if (PayloadParser.deserialize_document() == 200)
  {
    for (size_t i = 0; i < PayloadParser.limit; i++)
    {
      JsonObject root = PayloadParser.doc[i];
      std::string root_arrival_timestamp_scheduled = root["arrival_timestamp"]["scheduled"];
      std::string root_route_short_name = root["route"]["short_name"];

      JsonObject root_delay = root["delay"];
      int root_delay_minutes = root_delay["minutes"];

      JsonObject root_trip = root["trip"];
      std::string root_trip_headsign = root_trip["headsign"];

      Serial.println(root_route_short_name.c_str());
      Serial.println(root_trip_headsign.c_str());
      Serial.println(root_arrival_timestamp_scheduled.c_str());
    }
  }
}
