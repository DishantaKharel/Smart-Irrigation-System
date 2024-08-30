#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>


//Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "FXb85LIEEgcMYKc5EXy6LPIUEYoSKM7r"; //Enter your Auth token
char ssid[] = "Islington College"; //Enter your WIFI name
char pass[] = "I$LiNGT0N2024"; //Enter your WIFI password

#define BLYNK_TEMPLATE_ID "TMPL6TalNec9u"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation"

#include <BlynkSimpleEsp8266.h>

BlynkTimer timer;
bool Relay = 0;

//Define component pins
#define sensor A0
#define waterPump D3

// Threshold value for soil moisture level
const int moistureThreshold = 70; // Adjust this value according to your requirements

// Define a global variable to store the state of the automation switch
int automationState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH);
  lcd.init();
  lcd.backlight();

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  for (int a = 0; a <= 15; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();

  //Call the function
  timer.setInterval(100L, soilMoistureSensor);
}

//Get the button value
BLYNK_WRITE(V1) {
  Relay = param.asInt();

  if (Relay == 1) {
    digitalWrite(waterPump, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Motor is ON ");
  } else {
    digitalWrite(waterPump, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Motor is OFF");
  }
}


BLYNK_WRITE(V2) {       // This function gets called whenever the state of the virtual pin V2 changes
    automationState = param.asInt(); // Update the state of the automation switch
}

//Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(sensor);
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1;

  if (automationState == 1) { // If automation is enabled
    // Check if soil moisture level is below the threshold
    if (value < moistureThreshold) {
      digitalWrite(waterPump, LOW); // Turn on water pump
      lcd.setCursor(0, 1);
      lcd.print("Motor is ON ");
      Blynk.virtualWrite(V1, 1);  // Update button state in Blynk app
    } else {
        digitalWrite(waterPump, HIGH); // Turn off water pump
        lcd.setCursor(0, 1);
        lcd.print("Motor is OFF");
        Blynk.virtualWrite(V1, 0); // Update button state in Blynk app
    }
  }  

  Blynk.virtualWrite(V0, value);
  lcd.setCursor(0, 0);
  lcd.print("Moisture :");
  lcd.print(value);
  lcd.print(" ");
}

void loop() {
  Blynk.run(); // Run the Blynk library
  timer.run(); // Run the Blynk timer
}
