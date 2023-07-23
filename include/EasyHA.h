#ifndef EASYHA_H
#define EASYHA_H
#include <Arduino.h>
#include <ArduinoJson.h>

#if defined(ESP8266) 
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
#elif defined(ESP32)
    #include <WiFiClient.h>
    #include <HTTPClient.h>
#endif

struct SensorStruct {
    String value;
    String unit;
};

class EasyHA
{

    public:
        EasyHA(String url);
        EasyHA(String url, String haToken);

        SensorStruct readSensorValue(String sensorName);
        void setWifiClient(WiFiClient &client);
        void setHTTPClient(HTTPClient &client);

        void setBaseURL(String url); 
        String getBaseURL();
        void setHAToken(String haToken); 
        String getHAToken();
        
    private: 
        String _baseURL;
        String _HAToken;
        uint16 _port;
        WiFiClient *_wifiClient = new WiFiClient();
        HTTPClient *_httpClient = new HTTPClient();

        String httpCall(String url);

};

#endif