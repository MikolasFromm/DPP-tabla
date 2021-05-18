#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <time.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "setup.h"
#include <U8g2_for_TFT_eSPI.h>


TFT_eSPI tft = TFT_eSPI();
U8g2_for_TFT_eSPI u8f;

unsigned long Time = 0;

int width = 320; //RESOLUTION
int height = 240; //RESOLUTION

const char* SSID = WIFI_NAME; //specified in setup.h
const char* PASS = WIFI_PASS; //specified in setup.h


const String myAPI = X_HEADER_TOKEN;
const String TimeZone = "preferredTimezone=Europe%2FPrague";
const String ContentType = "application/json; charset=utf-8"; //CONTENT-SPECIFICATION
const String Zastavka = ZASTAVKA_CELA;
const String Sloupek = ZASTAVKOVY_SLOUPEK;
String serverName = "https://api.golemio.cz/v2/departureboards/?" + TimeZone + "&"; //DATA-SERVER
String serverConditions;
String serverPath;

const int limit = 5; //LIMIT-OF-OBJECTS

const char* ntpServer = "pool.ntp.org"; //TIME-SERVER
const long  gmtOffset_sec = 7200; //OFFSETS
const int   daylightOffset_sec = 0; //OFFSETS

TaskHandle_t data_task;
TaskHandle_t print_task;


void setup()
{

  wifi_tft_setup();

  input_data_check();

  // DUAL-CORE INIT

  xTaskCreatePinnedToCore(
    JSONread,
    "TASK_0",
    10000,
    NULL,
    0,
    &data_task,
    0
  );

  xTaskCreatePinnedToCore(
    JSONprint,
    "TASK_1",
    10000,
    NULL,
    0,
    &print_task,
    0
  );

}



void loop()
{
  vTaskDelete(NULL);
}

void wifi_tft_setup()
{
  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  u8f.begin(tft);

  WiFi.begin(SSID, PASS);
  Serial.println("Connecting");
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
  tft.drawString("Connecting...", 160, 60, 1);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
  tft.setTextPadding(250);
  tft.fillScreen(TFT_GREEN);
  tft.drawString("Succesfully", 160, 60, 1);
  tft.drawString("connected!", 160, 90, 1);
  tft.setTextSize(3);
  delay(2000);
  tft.fillScreen(TFT_BLACK);

  u8f.setFontMode(1);
  u8f.setFontDirection(0);
  u8f.setForegroundColor(TFT_WHITE);

}

void input_data_check()
{
  if (Zastavka == NULL)
  {
    serverConditions = Sloupek + "&limit=" + String(limit);
    serverPath = serverName + serverConditions;
  }

  if (Sloupek == NULL)
  {
    serverConditions = Zastavka + "&limit=" + String(limit);
    serverPath = serverName + serverConditions;
  }

  if (Sloupek == NULL && Zastavka == NULL)
  {
    u8f.setFont(u8g2_font_helvB12_te);
    u8f.setCursor(60, 130);
    u8f.print("Zastávka nebyla určena!");
    delay(15000);
  }

  if (myAPI == NULL)
  {
    u8f.setFont(u8g2_font_helvB12_te);
    u8f.setCursor(60, 130);
    u8f.print("Nebylo vloženo API!");
    delay(15000);
  }
}

void JSONread(void * parameter)
{
  for (;;)
  {
  }
}

void init_str_buffer(String buffer[])
{
  for (int i = 0; i < limit; i++)\
  {
    buffer[i] = "";
  }
}

void JSONprint(void * parameter)
{
  String old_final_stops_short[limit];
  String old_line[limit];

  init_str_buffer(old_final_stops_short);
  init_str_buffer(old_line);

  for (;;)
  {

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time");
      return;
    }

    int hour_now = timeinfo.tm_hour;
    int min_now = timeinfo.tm_min;
    int sec_now = timeinfo.tm_sec;


    if (WiFi.status() == WL_CONNECTED)
    {

      HTTPClient http;


      http.begin(serverPath.c_str());
      http.addHeader("x-access-token", myAPI);
      http.addHeader("content-type", ContentType);
      int httpResponseCode = http.GET();
      Serial.print("HTTP-CODE: ");
      Serial.println(httpResponseCode);
      
      // Small info text setting

      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(1);

      // Printing time "now"

      tft.setTextSize(2);
      tft.setTextDatum(BC_DATUM);
      tft.setTextPadding(tft.textWidth("00:00"));
      String time_to_print;


      if (hour_now < 10 && min_now < 10)
      {
        time_to_print = "0" + String(hour_now) + ":" + "0 " + String(min_now);
      }
      if (hour_now > 10 && min_now < 10)
      {
        time_to_print = String(hour_now) + ":" + "0 " + String(min_now);
      }
      if (hour_now < 10 && min_now > 10)
      {
        time_to_print = "0" + String(hour_now) + ":" + String(min_now);
      }
      if (hour_now > 10 && min_now > 10)
      {
        time_to_print = String(hour_now) + ":" + String(min_now);
      }

      tft.drawString(time_to_print, 160, 240, 1);
      tft.setTextSize(1);

      // Printing HTTP-CODE

      tft.setTextDatum(BL_DATUM);
      tft.setTextPadding(tft.textWidth("HTTP: 000"));
      String http_code;
      if (httpResponseCode == 200)
      {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        http_code = "HTTP: " + String(httpResponseCode);
      }else{
        tft.setTextColor(TFT_RED, TFT_BLACK);
        http_code = "HTTP: " + String(httpResponseCode) + " REBOOTING!";
      }
      tft.drawString(http_code, 0, 240, 1);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);

      // Printing "IP address"

      String ip = WiFi.localIP().toString();
      tft.setTextDatum(BR_DATUM);
      tft.setTextPadding(tft.textWidth("10.10.10.255"));
      tft.drawString(ip, 320, 240, 1);

      // DOWNLOADING, PARSING and PRINTING DEPARTURES

      if (httpResponseCode == 200)
      {
        String payload = http.getString();
        Serial.print("STRING: ");
        Serial.println(payload);
        DynamicJsonDocument doc(4400);
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }
        for (int i = 0; i < limit; i++)
        {

          JsonObject root = doc[i];

          String root_arrival_timestamp_scheduled = root["arrival_timestamp"]["scheduled"];
          const char* root_route_short_name = root["route"]["short_name"];

          JsonObject root_delay = root["delay"];
          int root_delay_minutes = root_delay["minutes"];

          JsonObject root_trip = root["trip"];
          const char* root_trip_headsign = root_trip["headsign"];

          String hour_predicted_string = root_arrival_timestamp_scheduled.substring(11, 13); // Fixed positions
          String minute_predicted_string = root_arrival_timestamp_scheduled.substring(14, 16); // Fixed positions
          String second_predicted_string = root_arrival_timestamp_scheduled.substring(17, 19); // Fixed positions

          String line = root_route_short_name;
          String final_stop = root_trip_headsign;

          int mezera = final_stop.indexOf(" ");
          int mezera2 = final_stop.indexOf(" ", mezera + 1);
          String final_stop_short;
          if (mezera != -1 && mezera > 4 && final_stop.length() > 18)
          {
            final_stop_short = final_stop.substring(0, 4) + ". " + final_stop.substring(mezera + 1);
            if (final_stop_short.length() > 18 && mezera2 - mezera > 6)
            {
              final_stop_short = final_stop.substring(0, 3) + ". " + final_stop.substring(mezera + 1, mezera + 4) + ". " + final_stop.substring(mezera2 + 1);
            }
          } else
          {
            final_stop_short = final_stop;
          }

          int hour_predicted = hour_predicted_string.toInt();
          int min_predicted = minute_predicted_string.toInt();
          int sec_predicted = second_predicted_string.toInt();
          int min_delay = root_delay_minutes;


          int min_remain = time_compar(hour_now, min_now, sec_now, hour_predicted, min_predicted, sec_predicted, min_delay);

          delay(500);

          if (old_line[i] != line)
          {
            u8f.setFont(u8g2_font_helvB18_te);
            u8f.setCursor(5, ((43 * i) + 30));
            tft.fillRect(5, ((43 * i) + 5), 50, 30, TFT_BLACK);
            u8f.print(line);
            old_line[i] = line;
          }

          if (old_final_stops_short[i] != final_stop_short)
          {
            u8f.setFont(u8g2_font_helvB12_te);
            u8f.setCursor(70, ((43 * i) + 30));
            tft.fillRect(70, ((43 * i) + 12), 200, 25, TFT_BLACK);
            u8f.print(final_stop_short);
            old_final_stops_short[i] = final_stop_short;
          }

          tft.setTextSize(2);
          tft.setTextDatum(BR_DATUM);
          if (min_remain < 1)
          {
            tft.setTextColor(TFT_ORANGE, TFT_BLACK);
            tft.setTextPadding(50);
            tft.drawString("<1", 290, ((43 * i) + 30), 1);
          }
          else {
            if (min_delay >= 1)
            {
              tft.setTextColor(TFT_RED, TFT_BLACK);
            }
            if (min_delay < 1)
            {
              tft.setTextColor(TFT_GREEN, TFT_BLACK);
            }
            tft.setTextPadding(45);
            tft.drawString(String(min_remain), 290, ((43 * i) + 30), 1);
          }

          tft.setTextSize(1);
          tft.setTextDatum(BR_DATUM);
          tft.setTextPadding(25);
          tft.drawString(" min", 320, ((43 * i) + 30), 1);
        }
        payload.remove(0);
      }
      if (httpResponseCode != 200)
      {
        delay(20000);
        ESP.restart();
      }
      http.end();
    }
    else
    {
      WiFi.reconnect();
    }
  }
}

int time_compar(int hour_now, int min_now, int sec_now, int hour_dep, int min_dep, int sec_dep, int time_delay)
{
  int min_remain;

  {
    if ((hour_dep - hour_now) < 0) //Pokud je hodina odjezdu až ve vyšší hodině
    {
      min_remain = min_dep + (60 * ((hour_dep - hour_now) + 24)) - min_now; //Převod do stejné hodiny, ale s časem > 60min

      if ((sec_dep - sec_now) > 0)
      {
        int sec_remain = sec_dep - sec_now;
        min_remain = min_remain + (sec_remain / 60);
      }

      if ((sec_dep - sec_now) < 0) //Pokud je sekunda odjezdu menší než aktuálního
      {
        int sec_remain = sec_dep + 60 - sec_now; //Převod sekundy na nižší minutu, ale s časem > 60sec
        min_remain--; //Odečtení jedné minuty
        min_remain = min_remain + (sec_remain / 60);
      }
    }
  }

  for (int i = 0; i < 10; i++)
  {
    if ((hour_dep - hour_now) > i) //Pokud je hodina odjezdu až ve vyšší hodině
    {
      min_remain = min_dep + (60 + (60 * i)) - min_now; //Převod do stejné hodiny, ale s časem > 60min

      if ((sec_dep - sec_now) > 0)
      {
        int sec_remain = sec_dep - sec_now;
        min_remain = min_remain + (sec_remain / 60);
      }

      if ((sec_dep - sec_now) < 0) //Pokud je sekunda odjezdu menší než aktuálního
      {
        int sec_remain = sec_dep + 60 - sec_now; //Převod sekundy na nižší minutu, ale s časem > 60sec
        min_remain--; //Odečtení jedné minuty
        min_remain = min_remain + (sec_remain / 60);
      }
    }
  }


  if ((hour_dep - hour_now) == 0) //Pokud je hodina odjezdu až ve stejné hodině
  {
    min_remain = min_dep - min_now; //Převod do stejné hodiny, ale s časem > 60min

    if ((sec_dep - sec_now) > 0)
    {
      int sec_remain = sec_dep - sec_now;
      min_remain = min_remain + (sec_remain / 60);
    }

    if ((sec_dep - sec_now) < 0) //Pokud je sekunda odjezdu menší než aktuálního
    {
      int sec_remain = sec_dep + 60 - sec_now; //Převod sekundy na nižší minutu, ale s časem > 60sec
      min_remain--; //Odečtení jedné minuty
      min_remain = min_remain + (sec_remain / 60);
    }
  }

  if (time_delay > 0)
  {
    min_remain = min_remain + time_delay;
  }

  return min_remain;
}
