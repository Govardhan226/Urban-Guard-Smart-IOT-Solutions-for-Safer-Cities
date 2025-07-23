#include <LiquidCrystal.h>
#include <DHT.h>

// Define the pins for the sensors
const int mq135Pin = A8;
const int dhtPin = 4;
const int waterSensor1Pin = A0;
const int waterSensor2Pin = A1;
const int ledPin = 13;         // Pin for onboard LED

// Location coordinates
const double incidentLatitude = 13.359275818903855;
const double incidentLongitude = 80.14081621102996;
const double evacuationLatitude = 13.120906252110908;
const double evacuationLongitude = 80.10190809383144;

// LCD Setup
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
DHT dht(dhtPin, DHT11);

void setup() {
  Serial.begin(9600);        // For debugging
  Serial1.begin(9600);       // For communication with ESP32 (changed to Serial1)
  Serial3.begin(9600);       // For GSM module communication
  lcd.begin(20, 4);          // Initialize the LCD immediately
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(100);

  dht.begin();               // Initialize the DHT sensor
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Turn on LED initially

  delay(20000);              // Wait for GSM module to register

  // Clear the LCD after initialization
  lcd.clear();

  // Check GSM module connection
  Serial.println("Checking GSM module connection...");
  Serial3.println("AT");
  delay(1000);
  while (Serial3.available()) {
    Serial.print(Serial3.readString());
  }

  // Set SMS mode to text
  Serial.println("Setting SMS mode to text...");
  Serial3.println("AT+CMGF=1");
}

void loop() {
    // Read temperature and humidity from DHT11
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        lcd.setCursor(0, 0);
        lcd.print("DHT Error");
        delay(1000);
        return;
    }

    // Read air quality and water levels
    int airQuality = analogRead(mq135Pin);
    int waterLevel1 = analogRead(waterSensor1Pin);
    int waterLevel2 = analogRead(waterSensor2Pin);

    // Compare water levels and send SMS alerts if necessary
    if (waterLevel1 > 500) { // Threshold for Water Sensor 1
        sendSMS("Officer", "Water level is increasing at: https://www.google.com/maps/search/?api=1&query=" + String(incidentLatitude) + "," + String(incidentLongitude));
        lcd.setCursor(0,3);
        lcd.print("W1 High: Msg Sent");
        delay(500); 
    }

    if (waterLevel1 > 500 && waterLevel2 > 500) { // Threshold for both Water Sensors
        sendSMS("Public", "Evacuate immediately from: https://www.google.com/maps/search/?api=1&query=" + String(evacuationLatitude) + "," + String(evacuationLongitude));
        lcd.setCursor(0,3);
        lcd.print("Evacuation Msg Sent");
        delay(500);
    }

    // Display data on the LCD
    lcd.setCursor(0,0);
    lcd.print("T: ");
    lcd.print(temperature);
    lcd.setCursor(10,0);
    lcd.print("H: ");
    lcd.print(humidity);

    lcd.setCursor(0,1);
    lcd.print("Air: ");
    lcd.print(airQuality);

    lcd.setCursor(0,2);
    lcd.print("W1: ");
    lcd.print(waterLevel1);
    lcd.setCursor(10,2);
    lcd.print("W2: ");
    lcd.print(waterLevel2);

    // Send data to ESP32 via Serial1
    String dataToSend = String(temperature) + "," + String(humidity) + "," + String(airQuality) + "," +
                        String(waterLevel1) + "," + String(waterLevel2) + "\n";
    
    Serial1.print(dataToSend); // Send data to ESP32 via Serial1
    Serial.println("Data sent to ESP32: " + dataToSend); // Print sent data to serial monitor

    delay(1000); // Wait for a second before sending again
}

void sendSMS(String recipient, String message) {
    Serial.println("Sending SMS...");
    
    String phoneNumber;
    
    if (recipient == "Officer") {
        phoneNumber = "+919962763252"; // Replace with officer's number
    } else if (recipient == "Public") {
        phoneNumber = "+919003983443"; // Replace with public's number or a broadcast number
    }

    Serial3.println("AT+CMGS=\"" + phoneNumber + "\"");
    
    delay(1000);
    
    while (Serial3.available()) {
        Serial.print(Serial3.readString());
    }

    Serial3.println(message); 
    
  
Serial3.println((char)26); 
delay (100);
}
