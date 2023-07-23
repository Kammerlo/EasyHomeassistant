#include "EasyHA.h"


EasyHA::EasyHA(String url) {
    this->_baseURL = url;
    this->_wifiClient = new WiFiClient();
    this->_httpClient = new HTTPClient();
}

EasyHA::EasyHA(String url, String haToken) {
    this->_baseURL = url;
    this->_HAToken = haToken;
}



void EasyHA::setBaseURL(String url) {
    this->_baseURL = url;
}

String EasyHA::getBaseURL() {
    return this->_baseURL;
}

void EasyHA::setHAToken(String haToken) {
    this-> _HAToken = haToken;
}

String EasyHA::getHAToken() {
  return this->_HAToken;
}

void EasyHA::setWifiClient(WiFiClient &client) {
    this->_wifiClient = &client;
}

void EasyHA::setHTTPClient(HTTPClient &client) {
  this->_httpClient = &client;
}

SensorStruct EasyHA::readSensorValue(String entity_id) {
    String url = this->_baseURL + "/api/states/sensor." + entity_id;
    String payload = this->httpCall(url);
    StaticJsonDocument<768> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print("DeserializeJson() failed");
        Serial.println(error.f_str());
    }
    SensorStruct values;
    values.unit = doc["attributes"]["unit_of_measurement"].as<String>();
    values.value = doc["state"].as<String>();
    return values;
    
    // currentSensorData.state = doc["state"].as<String>();
    // currentSensorData.unit = doc["attributes"]["unit_of_measurement"].as<String>();
}


String EasyHA::httpCall(String url) {

  String payload;
  if(_httpClient->begin(*_wifiClient,url)) {
    _httpClient->addHeader("Authorization", "Bearer " + this->getHAToken());
    _httpClient->addHeader("Content-Type", "application/json");
    int httpCode = _httpClient->GET();
    if (httpCode > 0) {
      if(httpCode == 200) {
        payload = _httpClient->getString();
      } else {
        // HTTP Error
        payload = "HTTP-Error: " + String(httpCode) + " payload:" + _httpClient->getString();
      }
      
    } else {
      Serial.println(httpCode);
      payload = _httpClient->errorToString(httpCode);
    }
  } else {
    payload = "HTTPClient Begin failed:" + _httpClient->getString();
  }
  _httpClient->end();
  return payload;
}