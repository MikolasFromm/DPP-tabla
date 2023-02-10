#include "payload_parser.hpp"

int payload_parser::input_data_check()
{
  if (this->Sloupek == "" && this->Zastavka == "")
  {
    return 10; // no argument given
  }
  else if (this->Sloupek != "" && this->Zastavka != "")
  {
    return 11; // collision arguments
  }
  else if (this->myAPI == "")
  {
    return 12; // no api-key given
  }
  else
  {
    std::ostringstream ss;
    ss << this->limit;
    this->serverConditions = this->Sloupek + this->Zastavka + "&limit=" + ss.str();
    this->serverPath = serverName + serverConditions;
    return -1;
  }
}

int payload_parser::deserialize_document()
{
  this->start_http_client();
  int request_https_code = this->http.GET();
  if (request_https_code == 200)
  {
    this->doc = SpiRamJsonDocument(1048576);
    DeserializationError json_err = deserializeJson(this->doc, this->http.getStream());
    if (json_err)
    {
      log_e("JSON deserialization failed: %s", json_err.c_str());
    }
    log_d("JsonDocument memory usage: %d", this->doc.memoryUsage());
  }
  this->end_http_client();
  return request_https_code;
}

void payload_parser::start_http_client()
{
  this->http.useHTTP10(true);
  this->http.begin(serverPath.c_str(), this->cert.c_str());
  this->http.addHeader("x-access-token", this->myAPI.c_str());
  this->http.addHeader("content-type",  this->ContentType.c_str());
}

void payload_parser::end_http_client()
{
  this->http.end();
}
