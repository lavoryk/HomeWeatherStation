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
MIIEsTCCA5mgAwIBAgIQCKWiRs1LXIyD1wK0u6tTSTANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0xNzExMDYxMjIzMzNaFw0yNzExMDYxMjIzMzNaMF4xCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xHTAbBgNVBAMTFFJhcGlkU1NMIFJTQSBDQSAyMDE4MIIBIjANBgkqhkiG9w0B
AQEFAAOCAQ8AMIIBCgKCAQEA5S2oihEo9nnpezoziDtx4WWLLCll/e0t1EYemE5n
+MgP5viaHLy+VpHP+ndX5D18INIuuAV8wFq26KF5U0WNIZiQp6mLtIWjUeWDPA28
OeyhTlj9TLk2beytbtFU6ypbpWUltmvY5V8ngspC7nFRNCjpfnDED2kRyJzO8yoK
MFz4J4JE8N7NA1uJwUEFMUvHLs0scLoPZkKcewIRm1RV2AxmFQxJkdf7YN9Pckki
f2Xgm3b48BZn0zf0qXsSeGu84ua9gwzjzI7tbTBjayTpT+/XpWuBVv6fvarI6bik
KB859OSGQuw73XXgeuFwEPHTIRoUtkzu3/EQ+LtwznkkdQIDAQABo4IBZjCCAWIw
HQYDVR0OBBYEFFPKF1n8a8ADIS8aruSqqByCVtp1MB8GA1UdIwQYMBaAFAPeUDVW
0Uy7ZvCj4hsbw5eyPdFVMA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEF
BQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADA0BggrBgEFBQcBAQQo
MCYwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBCBgNVHR8E
OzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRHbG9i
YWxSb290Q0EuY3JsMGMGA1UdIARcMFowNwYJYIZIAYb9bAECMCowKAYIKwYBBQUH
AgEWHGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwCwYJYIZIAYb9bAEBMAgG
BmeBDAECATAIBgZngQwBAgIwDQYJKoZIhvcNAQELBQADggEBAH4jx/LKNW5ZklFc
YWs8Ejbm0nyzKeZC2KOVYR7P8gevKyslWm4Xo4BSzKr235FsJ4aFt6yAiv1eY0tZ
/ZN18bOGSGStoEc/JE4ocIzr8P5Mg11kRYHbmgYnr1Rxeki5mSeb39DGxTpJD4kG
hs5lXNoo4conUiiJwKaqH7vh2baryd8pMISag83JUqyVGc2tWPpO0329/CWq2kry
qv66OSMjwulUz0dXf4OHQasR7CNfIr+4KScc6ABlQ5RDF86PGeE6kdwSQkFiB/cQ
ysNyq0jEDQTkfa2pjmuWtMCNbBnhFXBYejfubIhaUbEv2FOQB3dCav+FPg5eEveX
TVyMnGo=
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