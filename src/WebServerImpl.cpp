#include <WebServerImpl.h>
#if defined ESP32
  #include <WebServer.h>
  WebServer server(80);
#elif defined ESP8266
  #include <ESP8266WebServer.h>
  ESP8266WebServer server(80);
#endif

namespace 
{
  // const WeatherSensor* _pWeatherSensor = nullptr;
}
void handleRoot() 
{
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound() 
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) 
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void SetUpWebServer(const WeatherSensor* pWeatherSensor)
{
  // _pWeatherSensor = pWeatherSensor;
  server.on("/", handleRoot);

  server.on("/inline", []() 
  {
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/temperature", [pWeatherSensor]() 
  {
    String str(pWeatherSensor->GetTemperature());
    server.send(200, "text/plain", str.c_str() );
  });
  server.on("/pressure", [pWeatherSensor]() 
  {
    String str(pWeatherSensor->GetPressure());
    server.send(200, "text/plain", str.c_str() );
  });
  server.on("/humidity", [pWeatherSensor]() 
  {
    String str(pWeatherSensor->GetHumidity());
    server.send(200, "text/plain", str.c_str() );
  });

  server.onNotFound(handleNotFound);

  server.begin();
}

void HandleWebServerClient()
{
  server.handleClient();
}