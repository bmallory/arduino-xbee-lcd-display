#include <ArduinoJson.h>
#include <SoftwareSerial.h>


const String deviceID = "lcd1";


/**
 * Utilisé pour les IO XBee
 */
SoftwareSerial XBee(2, 3); // RX, TX
String incoming;


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

void setup()
{

  XBee.begin(9600);
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop()
{ 
  handleButton();
  handleXbee();
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
    Serial.println(incoming);
    JsonObject& decodedJson = jsonBuffer.parseObject(incoming); //DUH !!
    if (!decodedJson.success())
    {
      Serial.println("erreur de decodage, n'accepte que du JSON");
    }else{
      if( decodedJson[deviceID].asString() == NULL ){
        Serial.println("--ignored--");
      }else{
        /** 
         * envoi du texte vers le LCD 
         * activer le backlight durant 1 min.
         */
        Serial.println( decodedJson[deviceID].asString() );
      }
    }
  }
}
