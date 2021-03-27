#include <ArduinoHttpClient.h>
#include <b64.h>
#include <HttpClient.h>

#include <ArduinoJson.h>


#include <Arduino_JSON.h>
#include <JSON.h>
#include <JSONVar.h>

#include <WiFi.h>

#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1

const char* SSID = "OK1-BBTP50";
const char* PASS = "MikuljeBUH";

String serverName = "https://api.golemio.cz/v2/departureboards/";
const String myAPI = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbWFpbCI6Im1pa29sYXMuZnJvbW1AZ21haWwuY29tIiwiaWQiOjcxMCwibmFtZSI6bnVsbCwic3VybmFtZSI6bnVsbCwiaWF0IjoxNjE2MjcxMzg4LCJleHAiOjExNjE2MjcxMzg4LCJpc3MiOiJnb2xlbWlvIiwianRpIjoiZWFhM2EyMjktOWM2MS00OTU2LWE4NmUtNzM0MTVkMTdmZmU5In0.qMwNTzyjMjGJ6wk9S_EAh00up1b8o2ibrdnHj3MRjz4";
const String ContentType = "application/json; charset=utf-8";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

String API_DATA;

int limit = 4;
String limit_string = String(limit);

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

      String serverConditions = "?minutesBefore=10&minutesAfter=60&names=Sl%C3%A1nsk%C3%A1&preferredTimezone=Europe%2FPrague&limit=" + limit_string;
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
      
      for (JsonObject elem : doc.as<JsonArray>()) 
      {
      
        const char* arrival_timestamp_predicted = elem["arrival_timestamp"]["predicted"];
        const char* arrival_timestamp_scheduled = elem["arrival_timestamp"]["scheduled"];
      
        JsonObject delay = elem["delay"];
        bool delay_is_available = delay["is_available"]; // false, false, false, false
        // delay["minutes"] is null
        // delay["seconds"] is null
      
        const char* departure_timestamp_predicted = elem["departure_timestamp"]["predicted"];
        const char* departure_timestamp_scheduled = elem["departure_timestamp"]["scheduled"];
      
        const char* route_short_name = elem["route"]["short_name"]; // "98", "98", "98", "98"
        const char* route_type = elem["route"]["type"]; // "0", "0", "0", "0"
      
        JsonObject stop = elem["stop"];
        const char* stop_id = stop["id"]; // "U236Z1P", "U236Z2P", "U236Z1P", "U236Z2P"
        const char* stop_name = stop["name"]; // "Slánská", "Slánská", "Slánská", "Slánská"
        const char* stop_platform_code = stop["platform_code"]; // "A", "B", "A", "B"
        int stop_wheelchair_boarding = stop["wheelchair_boarding"]; // 1, 1, 1, 1
      
        JsonObject trip = elem["trip"];
        const char* trip_headsign = trip["headsign"]; // "Spojovací", "Sídliště Řepy", "Spojovací", "Sídliště ...
        const char* trip_id = trip["id"]; // "98_42_210131", "98_53_210306", "98_48_210131", "98_57_210306"
        // trip["is_canceled"] is null
        int trip_is_wheelchair_accessible = trip["is_wheelchair_accessible"]; // 2, 2, 2, 2
        Serial.print(arrival_timestamp_predicted[0]);
      }

      lastTime = millis();
      http.end();
            
    }

  }
}
