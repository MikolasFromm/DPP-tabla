#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <time.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

const char* SSID = "SSID";
const char* PASS = "PASSWD";

String serverName = "https://api.golemio.cz/v2/departureboards/";
const String myAPI = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbWFpbCI6Im1pa29sYXMuZnJvbW1AZ21haWwuY29tIiwiaWQiOjcxMCwibmFtZSI6bnVsbCwic3VybmFtZSI6bnVsbCwiaWF0IjoxNjE2MjcxMzg4LCJleHAiOjExNjE2MjcxMzg4LCJpc3MiOiJnb2xlbWlvIiwianRpIjoiZWFhM2EyMjktOWM2MS00OTU2LWE4NmUtNzM0MTVkMTdmZmU5In0.qMwNTzyjMjGJ6wk9S_EAh00up1b8o2ibrdnHj3MRjz4";
const String ContentType = "application/json; charset=utf-8";

const int limit = 5;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;



void setup()
{

  Serial.begin(38400);

  WiFi.begin(SSID, PASS);
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

}



void loop()
{

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }

  int hour_real = timeinfo.tm_hour;
  int minute_real = timeinfo.tm_min;
  int second_real = timeinfo.tm_sec;
  
      if (WiFi.status() == WL_CONNECTED)
        {

          HTTPClient http;

          String serverConditions = "?ids=U236Z1P&ids=U236Z4P&?minutesAfter=60&limit=" + String(limit);
          String serverPath = serverName + serverConditions;

          http.begin(serverPath.c_str());
          http.addHeader("x-access-token", myAPI);
          http.addHeader("content-type", ContentType);

          int httpResponseCode = http.GET();
          Serial.println(httpResponseCode);

          String payload = http.getString();
          // Serial.println(payload);


          DynamicJsonDocument doc(4000);

          DeserializationError error = deserializeJson(doc, payload);

          if (error)
            {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
              return;
            }
          Serial.println("Odjezdy: Slánská");
          for (int i = 0; i < limit; i++)
            {

              JsonObject root = doc[i];

              String root_arrival_timestamp_scheduled = root["arrival_timestamp"]["scheduled"];
              const char* root_route_short_name = root["route"]["short_name"];

              JsonObject root_delay = root["delay"];
              int root_delay_minutes = root_delay["minutes"];

              JsonObject root_trip = root["trip"];
              const char* root_trip_headsign = root_trip["headsign"];


              String line = root_route_short_name;
              String final_stop = root_trip_headsign;
              String hour_predicted_string = root_arrival_timestamp_scheduled.substring(11, 13); // Fixed positions
              String minute_predicted_string = root_arrival_timestamp_scheduled.substring(14, 16); // Fixed positions
              String second_predicted_string = root_arrival_timestamp_scheduled.substring(17, 19); // Fixed positions

              int hour_predicted_int = hour_predicted_string.toInt();
              int minute_predicted_int = minute_predicted_string.toInt();
              int second_predicted_int = second_predicted_string.toInt();
              
              Serial.print(line);
              Serial.print(" - ");
              Serial.print(final_stop);
              Serial.print(" - ");
              int min_remain = time_compar(hour_real, minute_real, second_real, hour_predicted_int, minute_predicted_int, second_predicted_int, root_delay_minutes);
              int min_delay = root_delay_minutes;
              if(min_remain < 1)
                {
                  Serial.println(" <1min");
                }
              else{
                  Serial.print(min_remain);
                Serial.println(" min");
              }

              tft.setTextDatum(ML_DATUM);
              tft.setTextColor(TFT_WHITE, TFT_BLACK);
              tft.setTextSize(2);
              tft.setTextPadding(50);              
              tft.drawString(line, 5, ((23 * i) + 15), 1);
              tft.setTextSize(1);
              tft.setTextPadding(150);  
              tft.drawString(final_stop, 50, ((23 * i) + 15), 1);
              tft.setTextSize(2);
              tft.setTextDatum(MR_DATUM);
              if(min_remain < 1){
                tft.setTextColor(TFT_BLACK, TFT_ORANGE);
                tft.setTextPadding(60);
                tft.drawString("<1 min", 240, ((23 * i) + 15), 1);
              }
              else{
                if(min_delay >= 1)
                {
                  tft.setTextColor(TFT_RED, TFT_BLACK);
                }
                if (min_delay < 1)
                {
                  tft.setTextColor(TFT_GREEN, TFT_BLACK);
                }
                tft.setTextPadding(80);
                tft.drawString(String(min_remain) + " min", 240, ((23 * i) + 15), 1);

              }
            }
            tft.setTextPadding(0);
            Serial.println("");

            http.end();
            payload.remove(0);
        }
}


int time_compar(int hour_now, int min_now, int sec_now, int hour_dep, int min_dep, int sec_dep, int delay)
  {
    int min_remain;
    if((hour_dep - hour_now) > 0) //Pokud je hodina odjezdu až ve vyšší hodině
      {
        min_remain = min_dep + 60 - min_now; //Převod do stejné hodiny, ale s časem > 60min

        if((sec_dep - sec_now) > 0)
          {
            int sec_remain = sec_dep - sec_now;
            min_remain = min_remain + (sec_remain / 60);
          }

        if((sec_dep - sec_now) < 0) //Pokud je sekunda odjezdu menší než aktuálního
          {
            int sec_remain = sec_dep + 60 - sec_now; //Převod sekundy na nižší minutu, ale s časem > 60sec
            min_remain--; //Odečtení jedné minuty
            min_remain = min_remain + (sec_remain / 60);
          }
      }


    if((hour_dep - hour_now) < 0) //Pokud je hodina odjezdu v nižší hodině (půlnoc a časy přes)
        {
          min_remain = min_dep + 60 - min_now; //Převod do stejné hodiny, ale s časem > 60min

          if((sec_dep - sec_now) > 0)
            { 
              int sec_remain = sec_dep - sec_now;
              min_remain = min_remain + (sec_remain / 60);
            }

          if((sec_dep - sec_now) < 0) //Pokud je sekunda odjezdu menší než aktuálního
          {
            int sec_remain = sec_dep + 60 - sec_now; //Převod sekundy na nižší minutu, ale s časem > 60sec
            min_remain--; //Odečtení jedné minuty
            min_remain = min_remain + (sec_remain / 60);
          }
        }
    if((hour_dep - hour_now) == 0) //Pokud je hodina odjezdu až ve stejné hodině
      {
        min_remain = min_dep - min_now; //Převod do stejné hodiny, ale s časem > 60min

        if((sec_dep - sec_now) > 0)
          {
            int sec_remain = sec_dep - sec_now;
            min_remain = min_remain + (sec_remain / 60);
          }

        if((sec_dep - sec_now) < 0) //Pokud je sekunda odjezdu menší než aktuálního
          {
            int sec_remain = sec_dep + 60 - sec_now; //Převod sekundy na nižší minutu, ale s časem > 60sec
            min_remain--; //Odečtení jedné minuty
            min_remain = min_remain + (sec_remain / 60);
          }
      }
    if(delay > 0)
    {
      min_remain = min_remain + delay;
    }
    
    return min_remain;
  }
