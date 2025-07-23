#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "Project";           // Replace with your Wi-Fi SSID
const char* password = "1234567890";    // Replace with your Wi-Fi password

// ThingSpeak credentials
const char* thingSpeakAPIKey = "4ZM3QI25EMIOO5PA"; // Replace with your ThingSpeak API key

void setup() {
  Serial.begin(9600);        // Initialize Serial Monitor for debugging
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // Initialize Serial2 for communication with Arduino Mega (GPIO16 as RX2, GPIO17 as TX2)

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("-> Connecting to WiFi...");
  }
  Serial.println("-> Connected to WiFi");
}

void loop() {
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n'); // Read data from Arduino Mega

    if (data.length() > 0) {
      Serial.println("Received data: " + data);

      // Parse the received data
      int commaIndex1 = data.indexOf(',');
      int commaIndex2 = data.indexOf(',', commaIndex1 + 1);
      int commaIndex3 = data.indexOf(',', commaIndex2 + 1);
      int commaIndex4 = data.indexOf(',', commaIndex3 + 1);
      int commaIndex5 = data.indexOf(',', commaIndex4 + 1);

      if (commaIndex1 != -1 && commaIndex2 != -1 && commaIndex3 != -1 && commaIndex4 != -1 && commaIndex5 != -1) {
        float temperature = data.substring(0, commaIndex1).toFloat();
        float humidity = data.substring(commaIndex1 + 1, commaIndex2).toFloat();
        int airQuality = data.substring(commaIndex2 + 1, commaIndex3).toInt();
        int waterLevel1 = data.substring(commaIndex3 + 1, commaIndex4).toInt();
        int waterLevel2 = data.substring(commaIndex4 + 1, commaIndex5).toInt();
        int soilMoistureState = data.substring(commaIndex5 + 1).toInt();

        // Print parsed values to serial monitor
        Serial.println("Parsed Data:");
        Serial.println("Temperature: " + String(temperature));
        Serial.println("Humidity: " + String(humidity));
        Serial.println("Air Quality: " + String(airQuality));
        Serial.println("Water Level W1: " + String(waterLevel1));
        Serial.println("Water Level W2: " + String(waterLevel2));
        Serial.println("Soil Moisture State: " + String(soilMoistureState));

        // Upload parsed data to ThingSpeak
        uploadToThingSpeak(temperature, humidity, airQuality, waterLevel1, waterLevel2, soilMoistureState);
      } else {
        Serial.println("Error parsing received data.");
      }
    }

    delay(1000); // Wait before checking again
  }
}

void uploadToThingSpeak(float temperature, float humidity, int airQuality, int waterLevel1, int waterLevel2, int soilMoistureState) {
  HTTPClient http;

  // Construct the URL for ThingSpeak API
  String url = "http://api.thingspeak.com/update?api_key=" + String(thingSpeakAPIKey) +
               "&field1=" + String(temperature) +
               "&field2=" + String(humidity) +
               "&field3=" + String(airQuality) +
               "&field4=" + String(waterLevel1) +
               "&field5=" + String(waterLevel2) +
               "&field6=" + String(soilMoistureState);

  http.begin(url); // Initialize HTTP client with the URL
  
  int httpResponseCode = http.GET(); // Send GET request
  
  if (httpResponseCode > 0) {
    Serial.println("-> Data uploaded to ThingSpeak successfully.");
    Serial.println("HTTP Response Code: " + String(httpResponseCode));
  } else {
    Serial.println("-> Error uploading data to ThingSpeak.");
    Serial.println("HTTP Response Code: " + String(httpResponseCode));
  }

  http.end(); // Close HTTP connection
}
