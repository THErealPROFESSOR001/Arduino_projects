// Copy this code into the Arduino IDE to upload it to your board
// Written by Virat Singh (therealprofessor0001@gmail.com)
// If any error occured contact above email for help

#include <LiquidCrystal_I2C.h> // Library for I2C-based LCD module
#include <SoftwareSerial.h>   // Library for SIM800L communication

// Initialize the LCD with I2C address 0x27 and dimensions 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define pin connections
const int relay_Pin = 8;        // Pin to control the water pump relay
const int moisture_sensor = A0; // Analog pin for soil moisture sensor
const int rain_Sesnor = A1;     // Analog pin for rain sensor

// Variables to hold sensor readings
int moisture_sensor_value;
int rain_Sesnor_value;

// Configure software serial for SIM800L (RX, TX pins)
SoftwareSerial sim800l(2, 3);

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize SIM800L communication
  sim800l.begin(9600);

  // Initialize the LCD and turn on the backlight
  lcd.init();
  lcd.backlight();

  // Configure relay pin as output
  pinMode(relay_Pin, OUTPUT);

  // Display initial message on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("START GO");
  lcd.setCursor(0, 1);
  lcd.print("GO");

  // Ensure the motor is off initially
  digitalWrite(relay_Pin, LOW);

  // Pause for 2 seconds to allow the user to see the startup message
  delay(2000);
}

void loop() {
  display_sensor_values(); // Read and display sensor values
  water_motor_start();    // Control the water motor based on sensor values
}

// Function to read and display sensor values
void display_sensor_values() {
  // Read the analog values from the sensors
  moisture_sensor_value = analogRead(moisture_sensor);
  rain_Sesnor_value = analogRead(rain_Sesnor);

  // Print sensor values to the Serial Monitor for debugging
  Serial.print("Moisture Level: ");
  Serial.println(moisture_sensor_value);
  Serial.print("Rain Sensor Value: ");
  Serial.println(rain_Sesnor_value);

  // Display sensor values on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moist: ");
  lcd.print(moisture_sensor_value);
  lcd.setCursor(0, 1);
  lcd.print("Rain: ");
  lcd.print(rain_Sesnor_value);

  // Wait 2 seconds before updating the display again
  delay(2000);
}

// Function to control the water motor and send SMS alerts
void water_motor_start() {
  // Check if it is raining (rain sensor value > 700)
  if (rain_Sesnor_value > 700) {
    // If moisture is low (value > 700), turn the motor OFF
    if (moisture_sensor_value > 700) {
      digitalWrite(relay_Pin, LOW); // Motor OFF
      sendSMS("Motor ON: Moisture Low. Moisture: " + String(moisture_sensor_value));
    } else {
      digitalWrite(relay_Pin, HIGH); // Motor ON
      sendSMS("Motor OFF: Conditions are sufficient. Moisture: " + String(moisture_sensor_value));
    }
  } else {
    // If it is not raining, turn the motor ON regardless of moisture
    digitalWrite(relay_Pin, HIGH); // Motor ON
    sendSMS("Motor OFF due to rain. Moisture: " + String(moisture_sensor_value));
  }
}

// Function to send SMS using the SIM800L module
void sendSMS(String message) {
  sim800l.println("AT");         // Check AT communication with SIM800L
  delay(1000);
  sim800l.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);

  // Set the recipient's phone number (replace with actual number)
  sim800l.println("AT+CMGS=\"+918447012566\"");
  delay(1000);

  // Send the message content
  sim800l.print(message);
  delay(100);

  // Send CTRL+Z (ASCII code 26) to indicate the end of the SMS
  sim800l.write(26);
  delay(3000);

  // Log a confirmation message to the Serial Monitor
  Serial.println("SMS sent!");
}
