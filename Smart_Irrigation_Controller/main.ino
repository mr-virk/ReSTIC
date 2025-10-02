#include <Blynk.h>
#include <dhtnew.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "id"
#define BLYNK_TEMPLATE_NAME "name"
#define BLYNK_AUTH_TOKEN "token"

/* Comment this out to disable prints and save space */
//#define BLYNK_PRINT Serial

// Replace with your WiFi credentials
const char* ssid = "wifi";
const char* pass = "pass";

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);  //Address, columns, rows 16x02

// Pin configuration
const int analogSensorPin = A0; // Analog pin for soil moisture sensor
const int relayPin1 = D7;        // Digital pin for relay control
DHTNEW dhtSensor(D3); // DHT Sensor Pin
const int rainPin = D6;

// Pump variables
bool isPumping = false;

//Flags
bool pumpOverrideFlag = false;
bool rainFlag = false;

//Variables
float moisture_percentage; //1.123
int desiredMoisture; //1
float humidityValue;
int temperatureValue;

//For Backgruound Tasks Tasks
unsigned long uptimeStartTime = 0;  // Records the start time of the device //postive
unsigned long previousTime = 0;
const unsigned long interval = 1000;  // 1000 millisecond = 1 second

//Costum Icons
byte drop[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000
};
byte temp[8] = {
  B01110,
  B01010,
  B01010,
  B01010,
  B01010,
  B10001,
  B10011,
  B01110
};

byte pump[8] = {
  B11000,
  B01011,
  B01010,
  B11111,
  B10001,
  B11111,
  B11111,
  B11111
};

byte moist[8] = {
  B11111,
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00000
};

byte rain[8] = {
  B00110,
  B01111,
  B11111,
  B00000,
  B10100,
  B00101,
  B10001,
  B00100
};

byte wifi[8] = {
  B01110,
  B10001,
  B00000,
  B00100,
  B01010,
  B00000,
  B00100,
  B00000
};


void setup() {
  Serial.begin(115200); //Baud Rate
  pinMode(relayPin1, OUTPUT);
  pinMode(analogSensorPin, INPUT);
  pinMode(rainPin, INPUT);

  //Relay inverted logic, High = Off, Low = On
  digitalWrite(relayPin1, HIGH);

  // Initializes and clears the LCD screen
  lcd.init(); //Start LCD
  lcd.backlight(); //Backlight On
  lcd.clear(); //Screen Clear

  // Creates the byte for the 3 custom characters
  lcd.createChar(0, drop);
  lcd.createChar(1, temp);
  lcd.createChar(2, pump);
  lcd.createChar(3, moist);
  lcd.createChar(4, rain);
  lcd.createChar(5, wifi);

  lcd.setCursor(0, 0);
  lcd.print(" Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print(" Wifi & Server ");

  uptimeStartTime = millis();  // Record the start time of the device

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); //Blynk

  delay(1500);

  lcd.setCursor(0, 0);
  lcd.print(" Connected to ");
  lcd.setCursor(0, 1);
  lcd.print(" Wifi & Server ");

  lcd.clear();
}

void loop() {
  Blynk.run();
  updateUptime();

  //Task Handler
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= interval) {
    previousTime = currentTime;
    //Tasks
    dhtSensorUpdate();
    soilMoistureCheck();
    rainCheck();
    pumpFunction();
    homeScreen();
  }
}

// void runTasks() {
//   unsigned long currentTime = millis();

//   if (currentTime - previousTime >= interval) {
//     previousTime = currentTime;

//     //Background Tasks
//     dhtSensorUpdate();
//     soilMoistureCheck();
//     rainCheck();
//     pumpFunction();
//     homeScreen();
//   }
// }

BLYNK_CONNECTED() {
  // Restore switch states from Blynk app on connection
  Blynk.syncVirtual(V1); //Virtual Pins
}

void updateUptime() {
  unsigned long currentMillis = millis();
  unsigned long uptimeSeconds = (currentMillis - uptimeStartTime) / 1000; //Convert millisec to sec

  // Update the uptime on Blynk app
  Blynk.virtualWrite(V1, uptimeSeconds);
}

BLYNK_WRITE(V15)
{
  // Read desired soil moisture level from Blynk slider
  desiredMoisture = param.asInt(); // assigning incoming value from pin V15 to a variable

  Serial.println("");
  Serial.println("( Updated Values Recived from Server. )");
  Serial.print("Moisture Input (%): ");
  Serial.println(desiredMoisture);

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Input Updated!");
  lcd.setCursor(0, 1);
  lcd.print("Moisture: ");
  lcd.setCursor(10, 1);
  lcd.print(desiredMoisture);
  lcd.setCursor(15, 1);
  lcd.print("%");

}

BLYNK_WRITE(V17) {
  if (param.asInt() == 1) {
    digitalWrite(relayPin1, LOW);
    Blynk.virtualWrite(V10, HIGH);
    Blynk.logEvent("pumpevent", "MANNUAL OVERRIDE: PUMP ON!");

    pumpOverrideFlag = true;

    //Serial.println("Mannual Override: ON");
  }
  if (param.asInt() == 0) {
    digitalWrite(relayPin1, HIGH);
    Blynk.virtualWrite(V10, LOW);
    Blynk.logEvent("pumpevent", "MANNUAL OVERRIDE: PUMP OFF!");

    pumpOverrideFlag = false;

    //Serial.println("Mannual Override: OFF");
  }
}

void dhtSensorUpdate() {
  int dhtValue = dhtSensor.read();

  if (dhtValue == DHTLIB_OK) {
    humidityValue = dhtSensor.getHumidity();
    temperatureValue = dhtSensor.getTemperature();

    Blynk.virtualWrite(V12, humidityValue);
    Blynk.virtualWrite(V13, temperatureValue);
  }
}

void soilMoistureCheck() {
  moisture_percentage = ( 100.00 - ( (analogRead(analogSensorPin) / 1023.00) * 100.00 ) ); //ADC to %

  //  Serial.print("Soil Moisture(in Percentage) = ");
  //  Serial.print(moisture_percentage);
  //  Serial.println("%");

  Blynk.virtualWrite(V11, moisture_percentage);
}

void rainCheck() {
  int rainVal = digitalRead(rainPin);

  //Sensor has inverted logic
  if (rainVal == LOW) {

    rainFlag = true;

    Blynk.virtualWrite(V18, HIGH);
    Blynk.logEvent("raindetected", "The Rain has been detected!");
  }
  if (rainVal == HIGH) {

    rainFlag = false;

    lcd.setCursor(11, 1);
    lcd.print(" ");

    Blynk.virtualWrite(V18, LOW);
  }
}

void pumpFunction() {
  //pump on
  if (moisture_percentage < desiredMoisture && moisture_percentage >= 3) {
    digitalWrite(relayPin1, LOW); // Turn on the relay

    isPumping = true;

    Blynk.virtualWrite(V10, HIGH);
    Blynk.logEvent("pumpevent", "Pump has been Started!");

    //Serial.println("Pump ON");
  }
  //for pump off
  if (rainFlag == true || moisture_percentage >= desiredMoisture) {
    digitalWrite(relayPin1, HIGH); // Turn off the relay

    isPumping = false;

    Blynk.virtualWrite(V10, LOW);
    Blynk.logEvent("pumpevent", "Pump has been Stoped!");

    //Serial.println("Pump OFF");
  }
}


void homeScreen() {
  lcd.clear();

  //Humidity
  lcd.setCursor(0, 0);
  lcd.write(0); //Drop Icon
  lcd.setCursor(2, 0);
  lcd.print(humidityValue);
  lcd.setCursor(5, 0);
  lcd.print("%");

  //Temperature
  lcd.setCursor(0, 1);
  lcd.write(1);
  lcd.setCursor(2, 1);
  lcd.print(temperatureValue);
  lcd.setCursor(5, 1);
  lcd.print("%");

  //Soil Moisture
  lcd.setCursor(8, 0);
  lcd.write(3);
  lcd.setCursor(10, 0);
  lcd.print(moisture_percentage);
  lcd.setCursor(15, 0);
  lcd.print("%");

  //Notifications
  lcd.setCursor(15, 1);
  lcd.write(5);

  lcd.setCursor(7, 1);
  lcd.write(isPumping ? 2 : ' ');

  lcd.setCursor(8, 1);
  lcd.write(pumpOverrideFlag ? 2 : ' ');

  lcd.setCursor(11, 1);
  lcd.write(rainFlag ? 4 : ' ');

  // //if (rainflag == true) {
  //   lcd.setCursor(11, 1);
  //   lcd.write(4);
  // }
}
