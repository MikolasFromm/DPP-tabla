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

const int limit = 4;

const char* line[limit] = {};
const char* time_predicted[limit] = {};
const char* final_stop[limit] = {};


void setup()
{

    Serial.begin(115200);

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

      String serverConditions = "?minutesBefore=10&minutesAfter=60&names=Sl%C3%A1nsk%C3%A1&preferredTimezone=Europe%2FPrague&limit=" + String(limit);
      String serverPath = serverName + serverConditions;
              
      http.begin(serverPath.c_str());
      http.addHeader("x-access-token", myAPI);
      http.addHeader("content-type", ContentType);

      int httpResponseCode = http.GET();
      Serial.println(httpResponseCode);

      String payload = http.getString();
      Serial.println(payload);

      DynamicJsonDocument doc(3072);
      
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

        const char* root_arrival_timestamp_predicted = root["arrival_timestamp"]["predicted"];
        const char* root_arrival_timestamp_scheduled = root["arrival_timestamp"]["scheduled"];

        JsonObject root_delay = root["delay"];
        bool root_delay_is_available = root_delay["is_available"]; // true
        int root_delay_minutes = root_delay["minutes"]; // 2
        int root_delay_seconds = root_delay["seconds"]; // 134

        const char* root_departure_timestamp_predicted = root["departure_timestamp"]["predicted"];
        const char* root_departure_timestamp_scheduled = root["departure_timestamp"]["scheduled"];

        const char* root_route_short_name = root["route"]["short_name"]; // "225"
        const char* root_route_type = root["route"]["type"]; // "3"

        JsonObject root_stop = root["stop"];
        const char* root_stop_id = root_stop["id"]; // "U236Z4P"#include <b64.h>
        const char* root_stop_name = root_stop["name"]; // "Slánská"
        const char* root_stop_platform_code = root_stop["platform_code"]; // "D"
        int root_stop_wheelchair_boarding = root_stop["wheelchair_boarding"]; // 0

        JsonObject root_trip = root["trip"];
        const char* root_trip_headsign = root_trip["headsign"]; // "Velká Ohrada"
        const char* root_trip_id = root_trip["id"]; // "225_277_210101"
        bool root_trip_is_canceled = root_trip["is_canceled"]; // false
        int root_trip_is_wheelchair_accessible = root_trip["is_wheelchair_accessible"]; // 1

        Serial.print(root_route_short_name);
        Serial.print(" ");
        Serial.print(root_trip_headsign);
        Serial.print(" ");
        Serial.println(root_arrival_timestamp_predicted);
        

      }

      lastTime = millis();
      http.end();
            
    }

  }
}
