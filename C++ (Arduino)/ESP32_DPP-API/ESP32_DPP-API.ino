#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino_JSON.h>
#include <JSON.h>
#include <JSONVar.h>
#include <WiFi.h>

const char* SSID = "OK1-BBTP50";
const char* PASS = "MikuljeBUH";

String serverName = "https://api.golemio.cz/v2/departureboards/";
const String myAPI = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbWFpbCI6Im1pa29sYXMuZnJvbW1AZ21haWwuY29tIiwiaWQiOjcxMCwibmFtZSI6bnVsbCwic3VybmFtZSI6bnVsbCwiaWF0IjoxNjE2MjcxMzg4LCJleHAiOjExNjE2MjcxMzg4LCJpc3MiOiJnb2xlbWlvIiwianRpIjoiZWFhM2EyMjktOWM2MS00OTU2LWE4NmUtNzM0MTVkMTdmZmU5In0.qMwNTzyjMjGJ6wk9S_EAh00up1b8o2ibrdnHj3MRjz4";
const String ContentType = "application/json; charset=utf-8";

unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

const int limit = 8;

const char* line[limit] = {};
const char* final_stop[limit] = {};
String hour_predicted[limit] = {};
String minute_predicted[limit] = {};
String second_predicted[limit] = {};


void setup()
{

    Serial.begin(38400);

    WiFi.begin(SSID, PASS);
    Serial.println("Connecting");

    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected to WiFi");
}


void loop()
{

  if ((millis() - lastTime) > timerDelay)
  {
    if(WiFi.status() == WL_CONNECTED)
    {
            
      HTTPClient http;

      String serverConditions = "?minutesAfter=60&names=Sl%C3%A1nsk%C3%A1&preferredTimezone=Europe%2FPrague&limit=" + String(limit);
      String serverPath = serverName + serverConditions;
              
      http.begin(serverPath.c_str());
      http.addHeader("x-access-token", myAPI);
      http.addHeader("content-type", ContentType);

      int httpResponseCode = http.GET();
      Serial.println(httpResponseCode);

      String payload = http.getString();
      Serial.println(payload);

      DynamicJsonDocument doc(8000);
      
      DeserializationError error = deserializeJson(doc, payload);
      
      if (error) 
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      
      for(int i = 0; i < limit; i++)
      {

        JsonObject root = doc[i];

        String root_arrival_timestamp_predicted = root["arrival_timestamp"]["predicted"];
        String root_arrival_timestamp_scheduled = root["arrival_timestamp"]["scheduled"];

        JsonObject root_delay = root["delay"];
        bool root_delay_is_available = root_delay["is_available"];
        int root_delay_minutes = root_delay["minutes"];
        int root_delay_seconds = root_delay["seconds"];

        const char* root_departure_timestamp_predicted = root["departure_timestamp"]["predicted"];
        const char* root_departure_timestamp_scheduled = root["departure_timestamp"]["scheduled"];

        const char* root_route_short_name = root["route"]["short_name"];
        const char* root_route_type = root["route"]["type"];

        JsonObject root_stop = root["stop"];
        const char* root_stop_id = root_stop["id"];
        const char* root_stop_name = root_stop["name"];
        const char* root_stop_platform_code = root_stop["platform_code"];
        int root_stop_wheelchair_boarding = root_stop["wheelchair_boarding"];

        JsonObject root_trip = root["trip"];
        const char* root_trip_headsign = root_trip["headsign"];
        const char* root_trip_id = root_trip["id"];
        bool root_trip_is_canceled = root_trip["is_canceled"];
        int root_trip_is_wheelchair_accessible = root_trip["is_wheelchair_accessible"];

        line[i] = root_route_short_name;
        final_stop[i] = root_trip_headsign;
        hour_predicted[i] = root_arrival_timestamp_predicted.substring(11, 13); // Fixed positions
        minute_predicted[i] = root_arrival_timestamp_predicted.substring(14, 16); // Fixed positions
        second_predicted[i] = root_arrival_timestamp_predicted.substring(17, 19); // Fixed positions
      }
      for(int i = 0; i < limit; i++)
       {
         
        Serial.print(line[i]);
        Serial.print(" - ");
        Serial.print(final_stop[i]);
        Serial.print(" - ");
        Serial.print(hour_predicted[i]);
        Serial.print(":");
        Serial.print(minute_predicted[i]);
        Serial.print(":");
        Serial.println(second_predicted[i]);
      }        

      lastTime = millis();
      http.end();
            
    }

  }
}
