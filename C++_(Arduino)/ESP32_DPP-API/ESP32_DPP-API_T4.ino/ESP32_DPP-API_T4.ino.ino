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


int width = 320; //RESOLUTION
int height = 240; //RESOLUTION

const char* SSID = WIFI_NAME; //specified in setup.h
const char* PASS = WIFI_PASS; //specified in setup.h

String serverName = "https://api.golemio.cz/v2/departureboards/"; //DATA-SERVER
const String myAPI = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbWFpbCI6Im1pa29sYXMuZnJvbW1AZ21haWwuY29tIiwiaWQiOjcxMCwibmFtZSI6bnVsbCwic3VybmFtZSI6bnVsbCwiaWF0IjoxNjE2MjcxMzg4LCJleHAiOjExNjE2MjcxMzg4LCJpc3MiOiJnb2xlbWlvIiwianRpIjoiZWFhM2EyMjktOWM2MS00OTU2LWE4NmUtNzM0MTVkMTdmZmU5In0.qMwNTzyjMjGJ6wk9S_EAh00up1b8o2ibrdnHj3MRjz4";
const String ContentType = "application/json; charset=utf-8"; //CONTENT-SPECIFICATION

const int limit = 5; //LIMIT-OF-OBJECTS

const char* ntpServer = "pool.ntp.org"; //TIME-SERVER
const long  gmtOffset_sec = 0; //OFFSETS
const int   daylightOffset_sec = 0; //OFFSETS

TaskHandle_t data_task;
TaskHandle_t print_task;


void setup()
{

  wifi_tft_setup();

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


}

void JSONread(void * parameter)
{
  for (;;)
  {
  }
}

void JSONprint(void * parameter)
{
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

      //String serverConditions = "?ids=U236Z1P&ids=U236Z4P&limit=" + String(limit);
      String serverConditions = "?names=And%C4%9Bl&limit=" + String(limit);
      String serverPath = serverName + serverConditions;

      http.begin(serverPath.c_str());
      http.addHeader("x-access-token", myAPI);
      http.addHeader("content-type", ContentType);

      int httpResponseCode = http.GET();
      Serial.println(httpResponseCode);
      Serial.println("");

      String payload = http.getString();
      // Serial.println(payload);


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
        String final_stop_short;
        if (mezera != -1 && mezera > 5 && tft.textWidth(final_stop) > 140 )
        {
          final_stop_short = final_stop.substring(0, 4) + ". " + final_stop.substring(mezera);
        }else
        {
          final_stop_short = final_stop;
        }

        int hour_predicted = hour_predicted_string.toInt();
        int min_predicted = minute_predicted_string.toInt();
        int sec_predicted = second_predicted_string.toInt();
        int min_delay = root_delay_minutes;


        int min_remain = time_compar(hour_now, min_now, sec_now, hour_predicted, min_predicted, sec_predicted, min_delay);
  
        u8f.setFont(u8g2_font_helvB12_te);
        u8f.setFontMode(1); 
        u8f.setFontDirection(0);
        u8f.setForegroundColor(TFT_WHITE);

        u8f.setCursor(5, ((43 * i) + 30));
        tft.fillRect(5, ((43 * i) + 12), 50, 25, TFT_BLACK);
        u8f.print(line);

        u8f.setCursor(70, ((43 * i) + 30));
        
        tft.fillRect(70, ((43 * i) + 12), 200, 25, TFT_BLACK);
        u8f.print(final_stop_short);

        tft.setTextSize(2);
        tft.setTextDatum(MR_DATUM);
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
        tft.drawString(" min", 320, ((43 * i) + 37), 1);
      }
      http.end();
      payload.remove(0);
    }
  }
}

int time_compar(int hour_now, int min_now, int sec_now, int hour_dep, int min_dep, int sec_dep, int delay)
{
  int min_remain;

  {
    if ((hour_dep - hour_now) < 0) //Pokud je hodina odjezdu až ve vyšší hodině
    {
      min_remain = min_dep + (60 * (hour_dep - hour_now) + 24) - min_now; //Převod do stejné hodiny, ale s časem > 60min

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

  if (delay > 0)
  {
    min_remain = min_remain + delay;
  }

  return min_remain;
}
