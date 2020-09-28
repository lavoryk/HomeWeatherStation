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
MIIFUTCCBDmgAwIBAgIQB5g2A63jmQghnKAMJ7yKbDANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0yMDA3MTYxMjI1MjdaFw0yMzA1MzEyMzU5NTlaMFkxCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxMzAxBgNVBAMTKlJhcGlkU1NMIFRMUyBE
ViBSU0EgTWl4ZWQgU0hBMjU2IDIwMjAgQ0EtMTCCASIwDQYJKoZIhvcNAQEBBQAD
ggEPADCCAQoCggEBANpuQ1VVmXvZlaJmxGVYotAMFzoApohbJAeNpzN+49LbgkrM
Lv2tblII8H43vN7UFumxV7lJdPwLP22qa0sV9cwCr6QZoGEobda+4pufG0aSfHQC
QhulaqKpPcYYOPjTwgqJA84AFYj8l/IeQ8n01VyCurMIHA478ts2G6GGtEx0ucnE
fV2QHUL64EC2yh7ybboo5v8nFWV4lx/xcfxoxkFTVnAIRgHrH2vUdOiV9slOix3z
5KPs2rK2bbach8Sh5GSkgp2HRoS/my0tCq1vjyLJeP0aNwPd3rk5O8LiffLev9j+
UKZo0tt0VvTLkdGmSN4h1mVY6DnGfOwp1C5SK0MCAwEAAaOCAgswggIHMB0GA1Ud
DgQWBBSkjeW+fHnkcCNtLik0rSNY3PUxfzAfBgNVHSMEGDAWgBQD3lA1VtFMu2bw
o+IbG8OXsj3RVTAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUHAwEG
CCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwNAYIKwYBBQUHAQEEKDAmMCQG
CCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wewYDVR0fBHQwcjA3
oDWgM4YxaHR0cDovL2NybDMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0R2xvYmFsUm9v
dENBLmNybDA3oDWgM4YxaHR0cDovL2NybDQuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0
R2xvYmFsUm9vdENBLmNybDCBzgYDVR0gBIHGMIHDMIHABgRVHSAAMIG3MCgGCCsG
AQUFBwIBFhxodHRwczovL3d3dy5kaWdpY2VydC5jb20vQ1BTMIGKBggrBgEFBQcC
AjB+DHxBbnkgdXNlIG9mIHRoaXMgQ2VydGlmaWNhdGUgY29uc3RpdHV0ZXMgYWNj
ZXB0YW5jZSBvZiB0aGUgUmVseWluZyBQYXJ0eSBBZ3JlZW1lbnQgbG9jYXRlZCBh
dCBodHRwczovL3d3dy5kaWdpY2VydC5jb20vcnBhLXVhMA0GCSqGSIb3DQEBCwUA
A4IBAQAi49xtSOuOygBycy50quCThG45xIdUAsQCaXFVRa9asPaB/jLINXJL3qV9
J0Gh2bZM0k4yOMeAMZ57smP6JkcJihhOFlfQa18aljd+xNc6b+GX6oFcCHGr+gsE
yPM8qvlKGxc5T5eHVzV6jpjpyzl6VEKpaxH6gdGVpQVgjkOR9yY9XAUlFnzlOCpq
sm7r2ZUKpDfrhUnVzX2nSM15XSj48rVBBAnGJWkLPijlACd3sWFMVUiKRz1C5PZy
el2l7J/W4d99KFLSYgoy5GDmARpwLc//fXfkr40nMY8ibCmxCsjXQTe0fJbtrrLL
yWQlk9VDV296EI/kQOJNLVEkJ54P
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