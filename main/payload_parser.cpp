#include "payload_parser.hpp"

int payload_parser::input_data_check(std::string& stop_name, int walktime_to_stop)
{
  if (stop_name == "")
  {
    return 10; // no argument given
  }
  else if (this->myAPI == "")
  {
    return 12; // no api-key given
  }
  else
  {
    std::ostringstream ss;
    ss << DOWNLOAD_LIMIT;
    this->serverConditions = stop_name + "&limit=" + ss.str() + "&minutesBefore=" + std::to_string(walktime_to_stop);
    this->serverPath = serverName + serverConditions;
    Serial.println(this->serverPath.c_str());
    return -1;
  }
}

int payload_parser::deserialize_document()
{
  this->start_http_client();
  Serial.print("Deserialization started: ");
  Serial.println(this->serverPath.c_str());
  int request_https_code = this->http.GET();
  if (request_https_code == 200)
  {
    this->doc = SpiRamJsonDocument(1048576);
    DeserializationError json_err = deserializeJson(this->doc, this->http.getStream());
    if (json_err)
    {
      log_e("JSON deserialization failed: %s", json_err.c_str()); // SHOULD BE EXCEPTION
    }
  }
  this->end_http_client();
  return request_https_code;
}

void payload_parser::start_http_client()
{
  this->http.useHTTP10(true);
  this->http.begin(this->serverPath.c_str(), this->cert.c_str());
  this->http.addHeader("x-access-token", this->myAPI.c_str());
  this->http.addHeader("content-type",  this->ContentType.c_str());
}

void payload_parser::end_http_client()
{
  this->http.end();
}
