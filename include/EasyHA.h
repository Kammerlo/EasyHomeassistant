#ifndef EASYHA_H
#define EASYHA_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <map>
#include <string>
#include <ctime>

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

template<size_t desiredSize> struct CalendarStruct {
    int entries;
    String start[desiredSize];
    String end[desiredSize];
    String summary[desiredSize];
};

class EasyHA
{

    public:
        EasyHA(String url);
        EasyHA(String url, String haToken);

        SensorStruct readSensorValue(String entityId);
        String updateSensorValue(String entityId,String state, std::map<String,String> attributes);
        String updateSensorValue(String entityId,String state, String unit);
        String updateSensorValue(String entityId,String state, String unit, String friendly_name);
        CalendarStruct<3> getCalendarEntries(String calendar_id, time_t start, time_t end);
        CalendarStruct<3> getCalendarEntries(String calendar_id, String start, String end);


        boolean deleteEntity(String entityId);
        
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