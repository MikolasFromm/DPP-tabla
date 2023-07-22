#ifndef payload_parser_hpp_
#define payload_parser_hpp_

#include "main_const.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <HTTPClient.h>
#include <ArduinoJson.h>

struct SpiRamAllocator {
  void* allocate(size_t size) {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  void deallocate(void* pointer) {
    heap_caps_free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size) {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
};

using SpiRamJsonDocument = BasicJsonDocument<SpiRamAllocator>;

class payload_parser
{
  public:
    int input_data_check(std::string& stop_names, int walktime_to_stop);
    int deserialize_document();
    void load_api_from_config(std::string& api);
    void start_http_client();
    void flush_json_doc() { this->doc.clear();}

    // Json document, default allocator missing
    SpiRamJsonDocument doc = SpiRamJsonDocument(1048576);

  private:

    void end_http_client();

    // http client for GET requests
    HTTPClient http;

    // getter parameters
    std::string myAPI;
    std::string TimeZone = TIMEZONE;
    const std::string ContentType = CONTENT_TYPE; //CONTENT-SPECIFICATION
    const std::string serverName = "https://api.golemio.cz/v2/departureboards/?" + TimeZone + "&"; //DATA-SERVER
    std::string serverConditions = "";
    std::string serverPath = "";

    // Golemio CERT
    std::string cert = "-----BEGIN CERTIFICATE-----\n" \
    "MIIFyzCCBLOgAwIBAgIQD2IvbyHC/11SH3I6HUfWLTANBgkqhkiG9w0BAQsFADBh\n" \
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
    "QTAeFw0yMjA1MDQwMDAwMDBaFw0zMTExMDkyMzU5NTlaMFwxCzAJBgNVBAYTAlVT\n" \
    "MRcwFQYDVQQKEw5EaWdpQ2VydCwgSW5jLjE0MDIGA1UEAxMrR2VvVHJ1c3QgR2xv\n" \
    "YmFsIFRMUyBSU0E0MDk2IFNIQTI1NiAyMDIyIENBMTCCAiIwDQYJKoZIhvcNAQEB\n" \
    "BQADggIPADCCAgoCggIBAOi2w4fkhoZPCI6L7nLMjvJTFg2rvXa7JPgQtpm9Ls4Z\n" \
    "9u2/SuiTDcjnGfjMYq9uTdBsiRjCC8fh3HsrPMCCAvfAf7bY349rOV4XWTGXZ2RS\n" \
    "UE20zKyhiF1Z+SkySD5+9yxzLNEyb+JXN8LLLcyB2Hw79jEq6v09+8zL5Ip3wFz9\n" \
    "+Uc3Tx4LVwTvW50pGMHFl3xpjO7iQS2RCkNcHHdqfEEkKy8EStVGA27aYYuHbgdx\n" \
    "ivjv0Axx3M4NrWfO8tGj8w0t8LhKDTuk/gFOI4klRcHRjcuH6giK6mkM3qpGGQLW\n" \
    "+Zc7Q93NFXalE5Qzn5/JESIcSPFDOezoAi9fMdtEa7Qj9/yCaUx5S14l66zlE1Od\n" \
    "y5hzpQBOlsw9KjJxsfpc4LQTB8aDaNjSqzLpwj6XlsRjaRon9GSS1q6HDYI3o8pR\n" \
    "x03xM1k7JTgiyyRO+84PVjLUOxy6u4SrEXRM0jdtxqnzfwW2CFsKo+5xHZB9xt5m\n" \
    "82zwUzY7+VOHEg8YpJxS2N6HR6QBvxo/6pgyfdmwAjiOGhA1GfHvQWf2vyHNguLq\n" \
    "1Jn4gr0b27HMZl6yqquv9O9XgDjPk147eym8GbN6AmBBke0HXR8fPwier1spgIoB\n" \
    "W3txZY6OiJr/JRl2n5MnUZ3QdyFfvzfkuBWwVCI7WI4gVJmhkOMeG9grhIRPm+zH\n" \
    "AgMBAAGjggGCMIIBfjASBgNVHRMBAf8ECDAGAQH/AgEAMB0GA1UdDgQWBBSltNbr\n" \
    "NsTna6bfxGQLASogBLhmIzAfBgNVHSMEGDAWgBQD3lA1VtFMu2bwo+IbG8OXsj3R\n" \
    "VTAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMC\n" \
    "MHYGCCsGAQUFBwEBBGowaDAkBggrBgEFBQcwAYYYaHR0cDovL29jc3AuZGlnaWNl\n" \
    "cnQuY29tMEAGCCsGAQUFBzAChjRodHRwOi8vY2FjZXJ0cy5kaWdpY2VydC5jb20v\n" \
    "RGlnaUNlcnRHbG9iYWxSb290Q0EuY3J0MEIGA1UdHwQ7MDkwN6A1oDOGMWh0dHA6\n" \
    "Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RDQS5jcmwwPQYD\n" \
    "VR0gBDYwNDALBglghkgBhv1sAgEwBwYFZ4EMAQEwCAYGZ4EMAQIBMAgGBmeBDAEC\n" \
    "AjAIBgZngQwBAgMwDQYJKoZIhvcNAQELBQADggEBAJ5ytcBRxwtzXW/S2tOySJu4\n" \
    "bhFRUuYRF91SMDUX8aX8Z/JIdLZb1+d6LIaiVkybFKYL8K2xual6/NL0tcI0T3Nw\n" \
    "/QNwS12NrfbS/th9aus7kiSbnNbkM2sc61vx9lF0qYklhJzSOkUPPSyq4Bdhg8G6\n" \
    "puAqrvQNqxNNMTTyMs5KNJdpLMEdIKdelM+9KKEMy9/jWGuLoNr8BvjkDx19VQSI\n" \
    "MCrwTFiQSC3sMkZQrCgZIwnQbf2ynOSMDutLoja5uKB7l+vbH2qSPFf3vD2HoTH7\n" \
    "S8+k0HfXb/f7ZSM5GDln3DTbBPI2qmmMiwFZJOMuYAQP1cyP8ywlhfdEdKVcW6E=\n" \
    "-----END CERTIFICATE-----\n";
};

#endif
