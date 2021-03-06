#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

const String deviceID = "lcd1";

/**
 * Utilisé pour les IO XBee
 */
SoftwareSerial XBee(2, 3); // RX, TX
String incoming;
String decoded;

/**
 * Utilisé pour le bouton
 */
const int buttonPin = 12;
// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
boolean backlightOn = false;


/**
 * utilisé pour le LCD
 */
//LiquidCrystal_I2C lcd(0x27,16,2); // TODO trouver la bonne adresse du LCD

#define I2C_ADDR    0x27 // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

int n = 1;

LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);


long backlightDelay = 60000;
long lastBacklight = 0;

void setup()
{
  XBee.begin(9600);
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(buttonPin, INPUT);
  lcd.begin (16,2);

 
// Switch on the backlight
lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);

lcd.home (); // go home

}

void loop()
{ 
  
  handleButton();
  handleXbee();
  handleLcdBacklight();
  digitalWrite(13, LOW); 
}


void handleButton(){
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        /**
         * activer le backlight durant 1 min
         */
              
        Serial.println("bouton activation backlight !!");
        lastBacklight = millis();
        lcd.setBacklight(HIGH);
        
        backlightOn = true;
      }
    }
  }
  lastButtonState = reading;
}

void handleXbee(){
  if (XBee.available())
  { // If data comes in from XBee, send it out to serial monitor
    StaticJsonBuffer<200> jsonBuffer;
    digitalWrite(13, HIGH);
    incoming = XBee.readString();
    
    JsonObject& decodedJson = jsonBuffer.parseObject(incoming); //DUH !!
    if (!decodedJson.success())
    {
      Serial.println("Send only JSON");
    }else{
      
      if( decodedJson[deviceID].asString() == NULL ){
        Serial.println("--ignored--");
      }else{
        /** 
         * envoi du texte vers le LCD 
         * activer le backlight durant 1 min.
         */
        decoded = decodedJson[deviceID].asString();
        if(decoded.length() > 32){
          decoded = decoded.substring(0,32);
        }
        lcd.clear();
        lcd.home();
        
        if( decoded.length() > 16){
          lcd.print( decoded.substring(0,16) );
          lcd.setCursor ( 0, 1 );
          lcd.print( decoded.substring(16) );  
        }else{
          lcd.print( decoded );
        }
        
        lastBacklight = millis();
        lcd.setBacklight(HIGH);
        
        backlightOn = true;

        
        
        Serial.println( decoded  + "5454"+ decoded.length() );
      }
    }
  }
}

void handleLcdBacklight(){
  if(backlightOn && (millis() - lastBacklight) > backlightDelay){
    Serial.println("suppression backlight !!");
   // lcd.noBacklight();
   lcd.setBacklight(LOW);
    backlightOn = false;
  }
}

