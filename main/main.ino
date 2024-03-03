// global libs
#include <utility>
#include <WiFi.h>
#include <time.h>
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>
#include <string>

// local implementations
#include "main_const.hpp"
#include "config_getter.hpp"
#include "payload_parser.hpp"
#include "payload_printer.hpp"
#include "time_getter.hpp"
#include "buttons.hpp"

// general setup
TFT_eSPI tft = TFT_eSPI();
U8g2_for_TFT_eSPI u8f;
bool deep_sleep = false;
int last_stop_index = 0;

// timing vars
unsigned long last_download_time;
unsigned long request_delay = 400;

// support libs construct
config_getter ConfigGetter;
payload_parser PayloadParser;
payload_printer PayloadPrinter;
time_getter TimeGetter;

// buttons
Button class_buttons[] = {BUTTON_1, BUTTON_2, BUTTON_3}; // left, center, right
constexpr int class_buttons_count = sizeof(class_buttons) / sizeof(class_buttons[0]);

// DualCore declaration
TaskHandle_t button_task;
TaskHandle_t data_task;

void setup()
{
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_37, 0);
  Serial.begin(115200);
  tft_setup();
  ConfigGetter.read_config(tft);
  PayloadParser.load_api_from_config(ConfigGetter.get_api_key());
  button_setup();
  wifi_setup();
  input_check();

  xTaskCreatePinnedToCore(
    ButtonRead,
    "TASK_BUTTONS",
    10000,
    NULL,
    0,
    &button_task,
    0
  );

  xTaskCreatePinnedToCore(
    JsonRead,
    "TASK_DATA",
    10000,
    NULL,
    0,
    &data_task,
    0
  );
}

void ButtonRead(void * parameter)
{
  for(;;)
  {
    if(class_buttons[2].check_it())
    {
      ConfigGetter.increment_stop_index();
      input_check();
    }
    if (class_buttons[0].check_it())
    {
        esp_deep_sleep_start();
    }
  }
}

void JsonRead(void * parameter)
{
  for(;;)
  {
    if (ConfigGetter.get_current_stop_index() != last_stop_index)
    {
      last_stop_index = ConfigGetter.get_current_stop_index();
      tft.fillScreen(TFT_BLACK);
      PayloadPrinter.clean_buffers();
    }
    unsigned long current_time = millis();

    if ((current_time - last_download_time) > request_delay)
    {
      print_payload();
      last_download_time = current_time;
    }
  }
}

void loop()
{
  vTaskDelete(NULL);
}

void button_setup()
{
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);  
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
  u8f.setFont(u8g2_font_helvB12_te);
}

void wifi_setup() // requires input_check to get SSID and PASSWORD
{
  ///<summary>Setup function to connect to given WiFi. Requires Serial and TFT already set-up</summary>
  WiFi.begin(ConfigGetter.get_ssid().c_str(), ConfigGetter.get_wifi_pass().c_str());

  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
  tft.drawString("Connecting...", 160, 60, 1);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Succesful connection notification
  tft.setTextPadding(250);
  tft.fillScreen(TFT_GREEN);
  tft.drawString("Succesfully", 160, 60, 1);
  tft.drawString("connected!", 160, 90, 1);
  tft.setTextSize(3);
  delay(2000);
  tft.fillScreen(TFT_BLACK);

}

void input_check() // possible to call repeatedly, always taking the current stop_name arguments
{
  PayloadParser.flush_json_doc();
  switch(PayloadParser.input_data_check(ConfigGetter.get_current_stop(), ConfigGetter.get_current_stop_walktime()))
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
    PayloadPrinter.print_payload(tft, u8f, PayloadParser, TimeGetter, ConfigGetter.get_current_stop_nickname());
  }
}
