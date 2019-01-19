#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ThingSpeak.h>
#include <Wire.h>
#include <SPI.h>

//[cpp]
// www.arduinesp.com
//
// Plot data on thingspeak.com using an ESP8266


#include <ESP8266WiFi.h>

// replace with your channel’s thingspeak API key,
String apiKey = "JWZOB2VGMDTOAWXR";
const char* ssid = "CHANGE INFO HERE";
const char* password = "CHANGE INFO HERE";

const char* server = "api.thingspeak.com";

WiFiClient client;

// Interval between data records in milliseconds.
const uint32_t SAMPLE_INTERVAL_MS = 30000;

// Time in millis for next data record.
uint32_t logTime;

float temp;
float humid;

//==============================================================================
//  Variables for Shinyei PPD42
int shinPin = 5;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
//==============================================================================
// DHT22 Variables
const int DHTPIN = 4;
DHT dht(DHTPIN, DHT22);

//==============================================================================
// Blink
int RXLED = 17;

void blink(int i) {
  for (int j = 0; j < i; j++) {
    digitalWrite(RXLED, LOW);
    delay(150);
    digitalWrite(RXLED, HIGH);
    delay(150);
  }
}

void setup() {
Serial.begin(115200);
delay(10);
dht.begin();

WiFi.begin(ssid, password);

Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");

pinMode(shinPin, INPUT);
pinMode(RXLED, OUTPUT);
  //TXLED0;  // Turn off the TX LED.

  Wire.begin();

  // Read any Serial data.
  do {
    delay(10);
  } 
  while (Serial.available() && Serial.read() >= 0);

  //dht.begin();

  // Blink for joy, things are working.
  blink(10);

  // Start on a multiple of the sample interval.
  // This will only be useful if we use a button and an ISR
  logTime = millis() / SAMPLE_INTERVAL_MS + 1;
  logTime *= SAMPLE_INTERVAL_MS;
}


void loop() {


temp = dht.readTemperature(true);
//Serial.print(temp);
//Serial.print(" *F ");
//humid = dht.readHumidity();

  
if (isnan(temp)) {
Serial.println("Failed to read from DHT sensor!");
return;
}

// Calculate Shinyei PM25 duty cycle and concentration.
ratio = lowpulseoccupancy / (sampletime_ms * 10.0); // Integer percentage 0=>100
concentration = (0.01237 * ratio) + 1.5;

if (client.connect(server,80)) { // "184.106.153.149" or api.thingspeak.com
String postStr = apiKey;
postStr +="&field1=";
postStr += String(temp); //Read value
postStr +="&field2=";
postStr += String(concentration);
postStr += "\r\n\r\n";

client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);

Serial.print("Temperature: ");
Serial.print(temp);
Serial.println(" degrees Fahrenheit");
//Serial.print(humid);
Serial.println("-- Sent to Thingspeak");
Serial.println("");
}
client.stop();

Serial.println("Waiting…");
Serial.println("");
// thingspeak needs minimum 30 sec delay between updates
delay(20000);
}

