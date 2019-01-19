#include <Adafruit_Sensor.h>
#include <DHT.h>
const int SHINPIN = 5;
const int DHTPIN = 4;
DHT dht(DHTPIN, DHT22);
int loopCount=0;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

void setup() {
  Serial.begin(9600);
  pinMode(SHINPIN,INPUT);
  starttime = millis();
  dht.begin();
}

void loop() {
  //Serial.println("Loop1");
  //Serial.println(loopCount++);
  duration = pulseIn(SHINPIN, LOW);
  Serial.println(duration);
  //Serial.println("Loop2");
  lowpulseoccupancy = lowpulseoccupancy+duration;
  //Serial.println("Loop3");
  //Serial.println(loopCount++);
  if ((millis()-starttime) > sampletime_ms) {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    Serial.print(lowpulseoccupancy);
    Serial.print(",");
    Serial.print(ratio);
    Serial.print(",");
    Serial.println(concentration);
    lowpulseoccupancy = 0;
    starttime = millis();

    float h = dht.readHumidity(); // Gets the values of the humidity
    float t = dht.readTemperature(); // Gets the values of the temperature

  
    // Printing the results on the serial monitor
    Serial.print("Temperature = ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print("    Humidity = ");
    Serial.print(h);
    Serial.println(" % ");
  }
}
