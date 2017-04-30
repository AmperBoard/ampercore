#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <string.h>
#define USE_SERIAL Serial

//#include <Scheduler.h>

ESP8266WiFiMulti WiFiMulti;

const int sensorIn = A0;
int mVperAmp = 66;

double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

int ledR = 16;
int ledB = 5;
int ledG = 4;
int ledMain = 0;


void setup(){
    USE_SERIAL.begin(115200);
    pinMode(ledR, OUTPUT);
    pinMode(ledB, OUTPUT);
    pinMode(ledG, OUTPUT);
    pinMode(ledMain, OUTPUT);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    /*for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }*/
    // We start by connecting to a WiFi network
    WiFiMulti.addAP("LAS NAVES OPEN", "");

    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");
    pinMode(LED_BUILTIN, OUTPUT);
    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    digitalWrite(ledMain, HIGH);
    digitalWrite(ledG, HIGH);
    delay(300);
    digitalWrite(ledG, LOW);
    digitalWrite(ledR, HIGH);
    delay(300);
    digitalWrite(ledG, HIGH);
    digitalWrite(ledR, LOW);
    delay(300);
    digitalWrite(ledG, LOW);
    digitalWrite(ledR, HIGH);
    delay(300);
    digitalWrite(ledG, HIGH);
    digitalWrite(ledR, LOW);
    

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
    //Scheduler.startLoop(loop2);
 //Serial.begin(115200);
}

void loop(){

  digitalWrite(ledB, HIGH);
  delay(300);
  digitalWrite(ledB, LOW);
  delay(300);
  digitalWrite(ledB, HIGH);
  delay(300);
  digitalWrite(ledB, LOW);

  Voltage = getVPP();
  VRMS = (Voltage/2.0) *0.707;  //root 2 is 0.707
  AmpsRMS = (VRMS * 1000)/mVperAmp;
  Serial.print(AmpsRMS-0.15);
  Serial.println(" Amps RMS");

 sendToServer("consumption=" + String(AmpsRMS, 2));
 delay(10000);
}

float getVPP()
{
  float result;
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the minimum sensor value*/
           minValue = readValue;
       }
   }
   
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;
      
   return result;
}

void sendToServer(String value){
  // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        http.begin("http://amper.herokuapp.com/real-time-data/post_from_arduino/?" + value); //HTTP
        http.addHeader("User-Agent", "amper");
        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }
  }
  

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;  
  
  void loop2() {
 analogWrite(ledMain, brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  // wait for 30 milliseconds to see the dimming effect
  delay(30);
}
