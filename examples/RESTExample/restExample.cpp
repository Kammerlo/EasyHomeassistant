#include <Arduino.h>
#include <EasyHA.h>

//************************************
// Replace with your wifi credentials
const char * ssid = "**************";
const char * password = "*************";

const char* HA_BASE_URL = "YOUR-HOMEASSISTANT";
const char* HA_TOKEN = "Your-LongLiveAccesToken"; // https://developers.home-assistant.io/docs/auth_api/

EasyHA easyHA(HA_BASE_URL,HA_TOKEN);


void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("WiFi Failed");
      while(1) {
          delay(1000);
      }
  }
  
  
}

void loop() {

  while(1)
  {
    SensorStruct values = easyHA.readSensorValue("Your-sensor-entity-id");
    Serial.println("Homeassistant Values:" + values.state + " " + values.attributes.at("unit_of_measurement"));
    // Writing data to new sensor
    easyHA.updateSensorValue("test-entity",values.state,values.attributes.at("unit_of_measurement"));
    // and deleting it afterwards
    easyHA.deleteEntity("test-entity");

    CalendarStruct calendarEntries = easyHA.getCalendarEntries("calendar.default_calendar","2023-07-26T00:00:00","2023-07-29T00:00:00");

    delay(5 * 1000); 
    
  }
}