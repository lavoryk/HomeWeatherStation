#include <Currency.h>
#if defined ESP32
#include <HTTPClient.h>
#elif defined ESP8266
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
using namespace BearSSL;
#endif

#include <WiFiClient.h>
#include <ArduinoJson.h>

void Currency::ReadCurrency()
{
  const char* rootCACertificate = R"EOF(
-----BEGIN CERTIFICATE-----
MIIESTCCAzGgAwIBAgITBn+UV4WH6Kx33rJTMlu8mYtWDTANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MTAyMjAwMDAwMFoXDTI1MTAxOTAwMDAwMFowRjEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEVMBMGA1UECxMMU2VydmVyIENB
IDFCMQ8wDQYDVQQDEwZBbWF6b24wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK
AoIBAQDCThZn3c68asg3Wuw6MLAd5tES6BIoSMzoKcG5blPVo+sDORrMd4f2AbnZ
cMzPa43j4wNxhplty6aUKk4T1qe9BOwKFjwK6zmxxLVYo7bHViXsPlJ6qOMpFge5
blDP+18x+B26A0piiQOuPkfyDyeR4xQghfj66Yo19V+emU3nazfvpFA+ROz6WoVm
B5x+F2pV8xeKNR7u6azDdU5YVX1TawprmxRC1+WsAYmz6qP+z8ArDITC2FMVy2fw
0IjKOtEXc/VfmtTFch5+AfGYMGMqqvJ6LcXiAhqG5TI+Dr0RtM88k+8XUBCeQ8IG
KuANaL7TiItKZYxK1MMuTJtV9IblAgMBAAGjggE7MIIBNzASBgNVHRMBAf8ECDAG
AQH/AgEAMA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUWaRmBlKge5WSPKOUByeW
dFv5PdAwHwYDVR0jBBgwFoAUhBjMhTTsvAyUlC4IWZzHshBOCggwewYIKwYBBQUH
AQEEbzBtMC8GCCsGAQUFBzABhiNodHRwOi8vb2NzcC5yb290Y2ExLmFtYXpvbnRy
dXN0LmNvbTA6BggrBgEFBQcwAoYuaHR0cDovL2NydC5yb290Y2ExLmFtYXpvbnRy
dXN0LmNvbS9yb290Y2ExLmNlcjA/BgNVHR8EODA2MDSgMqAwhi5odHRwOi8vY3Js
LnJvb3RjYTEuYW1hem9udHJ1c3QuY29tL3Jvb3RjYTEuY3JsMBMGA1UdIAQMMAow
CAYGZ4EMAQIBMA0GCSqGSIb3DQEBCwUAA4IBAQCFkr41u3nPo4FCHOTjY3NTOVI1
59Gt/a6ZiqyJEi+752+a1U5y6iAwYfmXss2lJwJFqMp2PphKg5625kXg8kP2CN5t
6G7bMQcT8C8xDZNtYTd7WPD8UZiRKAJPBXa30/AbwuZe0GaFEQ8ugcYQgSn+IGBI
8/LwhBNTZTUVEWuCUUBVV18YtbAiPq3yXqMB48Oz+ctBWuZSkbvkNodPLamkB2g1
upRyzQ7qDn1X8nn8N8V7YJ6y68AtkHcNSRAnpTitxBKjtKPISLMVCx7i4hncxHZS
yLyKQXhw2W2Xs0qLeC1etA+jTGDK4UfLeC0SF7FSi8o5LL21L8IzApar2pR/
-----END CERTIFICATE-----
)EOF";

    // WiFiClient client;    
    std::unique_ptr<WiFiClientSecure> client(new WiFiClientSecure);
#if defined ESP32
    client->setCACert(rootCACertificate);
#elif defined ESP8266
    BearSSL::X509List cert(rootCACertificate);
    client->setTrustAnchors(&cert);
#endif

    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    char line[64] = {0};
#if 0
    {
      time_t rawtime;
      time (&rawtime);
      struct tm* timeinfo = localtime (&rawtime);
      strftime(line, 64, "%d.%m.%Y", timeinfo);
    }
    String request = "https://api.privatbank.ua/p24api/exchange_rates?json&date=";
    request += line;
#endif
    String request = "https://api.privatbank.ua/p24api/pubinfo?json&exchange&coursid=5";
    Serial.print("[HTTP] ");
    Serial.println(request);
    if (http.begin(*client, request))
    {
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      // httpCode will be negative on error
      if (httpCode > 0) 
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
        {
          String payload = http.getString();
          Serial.print("legnth : ");
          Serial.println(payload.length());
          // Serial.println(payload);
          {
            // https://arduinojson.org/v6/assistant/
            const size_t capacity = JSON_ARRAY_SIZE(4) + 4*JSON_OBJECT_SIZE(4) + 220;
            DynamicJsonDocument doc(capacity);
            deserializeJson(doc, payload);
            for (size_t i = 0; i < doc.size(); ++i)
            {
              JsonObject root_0 = doc[i];
              if (root_0.containsKey("ccy") && strcmp(root_0["ccy"], "USD") == 0)
              {
                exchangeRate_24_saleRateNB = 0.0f;
                exchangeRate_24_purchaseRateNB = 0.0f;
                if (root_0.containsKey("sale"))
                {
                  exchangeRate_24_saleRate = root_0["sale"];
                }
                if (root_0.containsKey("buy"))
                {
                  exchangeRate_24_purchaseRate = root_0["buy"];
                }
                sprintf(line, "NB%5.2f PV%5.2f/%5.2f", exchangeRate_24_saleRateNB, exchangeRate_24_purchaseRate, exchangeRate_24_saleRate);
                Serial.println(line);
                break;
              }
            }
          }
        }
      } 
      else 
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } 
    else 
    {
      Serial.printf("[HTTP} Unable to connect\n");
    }
}