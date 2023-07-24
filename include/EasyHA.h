#ifndef EASYHA_H
#define EASYHA_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <map>
#include <string>

#if defined(ESP8266) 
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
#elif defined(ESP32)
    #include <WiFiClient.h>
    #include <HTTPClient.h>
#endif

struct SensorStruct {
    String state;
    std::map<String,String> attributes;
};

class EasyHA
{

    public:
        EasyHA(String url);
        EasyHA(String url, String haToken);

        SensorStruct readSensorValue(String entity_id);
        String updateSensorValue(String entity_id,String state, std::map<String,String> attributes);
        String updateSensorValue(String entity_id,String state, String unit);
        String updateSensorValue(String entity_id,String state, String unit, String friendly_name);
        boolean deleteEntity(String entity_id);
        
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

        String httpGetCall(String url);
        String httpPostCall(String url, String payload);
        int httpDeleteCall(String url);
        String constructHAJson(String state,std::map<String,String> attributes);

        String httpResponseHandling(int httpCode);
        void prepareHTTPHeader();

};

#endif