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

/**
 * Gets the calendar entries from Homeassistant. Take care to set up NTP for your ESP. 
 * JSONDocumentSize is set for three calendar entries. 
 * Calculate needed size with ArduinoJSON Assistant https://arduinojson.org/v6/assistant/#/step1
 * 
 * @param calendar_id 
 * @param start Needed Format %Y-%m-%dT%H-%M-%S -> e.g. 2023-07-26T17:25:00
 * @param end Needed Format %Y-%m-%dT%H-%M-%S -> e.g. 2023-07-26T17:25:00
 * @return 
*/
CalendarStruct EasyHA::getCalendarEntries(String calendar_id, time_t start, time_t end, int dynamicJsonDocumentBufferSize = 1154) {
  char startDate[20];
  char endDate[20];
  strftime(startDate,sizeof(startDate),"%Y-%m-%dT%H-%M-%S", localtime(&start));
  strftime(endDate,sizeof(endDate),"%Y-%m-%dT%H-%M-%S", localtime(&end));
  return getCalendarEntries(calendar_id,String(startDate),String(endDate),dynamicJsonDocumentBufferSize);
}

/**
 * Gets the calendar entries from Homeassistant. Take care to set up NTP for your ESP. 
 * JSONDocumentSize is set for three calendar entries. 
 * Calculate needed size with ArduinoJSON Assistant https://arduinojson.org/v6/assistant/#/step1
 * 
 * @param calendar_id 
 * @param start Needed Format %Y-%m-%dT%H-%M-%S -> e.g. 2023-07-26T17:25:00
 * @param end Needed Format %Y-%m-%dT%H-%M-%S -> e.g. 2023-07-26T17:25:00
 * @return 
*/
CalendarStruct EasyHA::getCalendarEntries(String calendar_id, String start, String end, int dynamicJsonDocumentBufferSize = 1154) {
  String url = this->_baseURL + "/api/calendars/" + calendar_id + "?start=" + start + "&end=" + end;
  String payload = httpGetCall(url);
  DynamicJsonDocument doc(dynamicJsonDocumentBufferSize);
  DeserializationError error = deserializeJson(doc,payload);
  if(doc.overflowed()) {
    Serial.println("JSON Document overflowed.");
  }
  CalendarStruct calendarStruct;
  calendarStruct.entries = doc.size();
  calendarStruct.start = new String[calendarStruct.entries];
  calendarStruct.end = new String[calendarStruct.entries];
  calendarStruct.summary = new String[calendarStruct.entries];
  for(int i = 0; i < doc.size();i++){
    calendarStruct.start[i] = doc[i]["start"]["date"].as<String>();
    calendarStruct.end[i] = doc[i]["end"]["date"].as<String>();
    calendarStruct.summary[i] = doc[i]["summary"].as<String>();
  }

  return calendarStruct;
}

SensorStruct EasyHA::readSensorValue(String entityID) {
    String url = this->_baseURL + "/api/states/sensor." + entityID;
    String payload = this->httpGetCall(url);
    StaticJsonDocument<768> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print("DeserializeJson() failed");
        Serial.println(error.f_str());
    }
    SensorStruct values;
    JsonObject attributes = doc["attributes"];
    for(JsonPair pair : attributes) {
      
      values.attributes.insert(std::pair<String,String>(pair.key().c_str(), pair.value().as<String>()));
    }

    values.state = doc["state"].as<String>();
    return values;
}

String EasyHA::updateSensorValue(String entityID, String state, std::map<String,String> attributes) {
  String url = this->_baseURL + "/api/states/sensor." + entityID;
  SensorStruct values;
  String payload = constructHAJson(state,attributes);
  return httpPostCall(url,payload);
}

String EasyHA::updateSensorValue(String entityID,String state, String unit) {
  std::map<String,String> attributes;
  attributes.insert(std::pair<String,String>("unit_of_measurement",unit));
  return updateSensorValue(entityID,state,attributes);
}

String EasyHA::updateSensorValue(String entityID,String state, String unit, String friendly_name){
  std::map<String,String> attributes;
  attributes.insert(std::pair<String,String>("unit_of_measurement",unit));
  attributes.insert(std::pair<String,String>("friendly_name",friendly_name));
  return updateSensorValue(entityID,state,attributes);
}

boolean EasyHA::deleteEntity(String entityID) {
  String url = this->_baseURL + "/api/states/sensor." + entityID;
  int httpCode = httpDeleteCall(url);
  if(httpCode == 200) {
    return true;
  } else {
    return false;
  }
}


String EasyHA::constructHAJson(String state, std::map<String,String> attributes) {
  String payload = "";
  payload.concat("{\"state\":\"");
  payload.concat(state.c_str());
  payload.concat("\",\"attributes\": {");
  for(auto const& [key, val] : attributes) {
    payload.concat("\"" + key + "\": \"" + val + "\",");
  }
  payload = payload.substring(0,payload.length() - 1); // getting rid of the last ,
  payload.concat('}');
  payload.concat('}');
  return payload;
}


String EasyHA::httpGetCall(String url) {

  String response;
  if(_httpClient->begin(*_wifiClient,url)) {
    prepareHTTPHeader();
    int httpCode = _httpClient->GET();
    response = this->httpResponseHandling(httpCode);
  } else {
    response = "HTTPClient Begin failed:" + _httpClient->getString();
  }
  _httpClient->end();
  return response;
}

String EasyHA::httpPostCall(String url, String payload) {
  String response;
  if(_httpClient->begin(*_wifiClient,url)) {
    prepareHTTPHeader();
    int httpCode = _httpClient->POST(payload);
    response = this->httpResponseHandling(httpCode);
  } else {
    response = "HTTPClient Begin failed:" + _httpClient->getString();
  }
  _httpClient->end();
  return response;
}

int EasyHA::httpDeleteCall(String url) {
  int httpCode;
  if(_httpClient->begin(*_wifiClient,url)) {
    prepareHTTPHeader();
    httpCode = _httpClient->DELETE();
  } else {
    httpCode = -1;
  }
  _httpClient->end();
  return httpCode;
}

String EasyHA::httpResponseHandling(int httpCode) {
  String payload;
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
  return payload;
}

void EasyHA::prepareHTTPHeader() {
  _httpClient->addHeader("Authorization", "Bearer " + this->getHAToken());
  _httpClient->addHeader("Content-Type", "application/json");
}