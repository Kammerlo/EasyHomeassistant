# EasyHomeassistant

A small library for handling HomeAssistant easier in your PlatformIO projects. Repository is still work-in-progress.

The REST-API of Homeassistant is implemented. Basic CRUD Operations are implemented (getSensorValue, updateSensorValue & deleteSensor).
For the full API see the [official Home Assistant Documentation](https://developers.home-assistant.io/docs/api/rest/). 

### Currently implemented:
- (GET,POST,DELETE) -> /api/states/\<entity_id>
- (GET) -> /api/calendars 

**Still ToDo:**
- Calendars for more than 3 entries at a time
- MQTT API


**Feel free to suggest something you need**
