#include "payload_parser.hpp"

int payload_parser::input_data_check(std::string& stop_name, int walktime_to_stop, int display_index)
{
  if (stop_name == "")
  {
    return 10; // no argument given
  }
  else if (this->myAPI == "")
  {
    return 12; // no api-key given
  }
  else if (display_index > 1 || display_index < 0)
  {
    return 13; // invalid display index
  }
  else
  {
    this->serverConditions = stop_name + "&limit=" + std::to_string(number_of_diplayed_rows+(display_index*number_of_diplayed_rows)) + "&minutesBefore=" + std::to_string(walktime_to_stop) + "&offset=" + std::to_string(display_index*number_of_diplayed_rows);
    this->serverPath = serverName + serverConditions;
    Serial.println(this->serverPath.c_str());
    return -1;
  }
}

int payload_parser::deserialize_document()
{
  try 
  { 
    this->start_http_client();
    Serial.print("Deserialization started: ");
    Serial.println(this->serverPath.c_str());
    int request_https_code = this->http.GET();
    if (request_https_code == 200)
    {
      this->doc = SpiRamJsonDocument(1048576);
      String payload = this->http.getString();
      DeserializationError json_err = deserializeJson(this->doc, payload);
      if (json_err)
      {
        log_e("JSON deserialization failed: %s", json_err.c_str()); // SHOULD BE EXCEPTION
      }
    }
    else
    {
      Serial.print("status_code: ");
      Serial.println(request_https_code);
    }
    this->end_http_client();
    return request_https_code;
  } 
  catch (const std::exception& e) 
  { 
    Serial.println(e.what());
  }
}

void payload_parser::load_api_from_config(std::string& api)
{
  this->myAPI = api;
}

void payload_parser::start_http_client()
{
//  this->http.useHTTP10(true);
//  this->http.begin(this->serverPath.c_str(), this->cert.c_str());
  this->http.begin(this->serverPath.c_str());
  this->http.addHeader("x-access-token", this->myAPI.c_str());
  this->http.addHeader("content-type",  this->ContentType.c_str());
}

void payload_parser::end_http_client()
{
  this->http.end();
}
