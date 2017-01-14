// DeviceID: 
// AccessToken: 

// Test in terminal by using command:
// curl https://api.particle.io/v1/devices/YOUR_DEVICE_ID/Temperature?access_token=YOUR_TOKEN

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_DHT.h"

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#define DHTPIN 2     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11		// DHT 11
#define DHTTYPE DHT22		// DHT 22 (AM2302)
//#define DHTTYPE DHT21		// DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

#define PUBLISH_TOPIC   "HKSValues"
#define DELAY_LOOP      5000
#define DELAY_PUBLISH   500
#define MAX_DATA        63

// Temperature
float currentTemperature;
// Pressure
float currentHumidity;
// Light is on or off
bool isOn;

String tempStr;
String humStr;

// Publish sensor value
void publish(char* name, double value){
    char szData[MAX_DATA];

    sprintf(szData, "%s=%2.02f", name, value);

    Particle.publish(PUBLISH_TOPIC, szData);
}



void setup() {

    Particle.variable("Temperature",tempStr);
    Particle.variable("Humidity", humStr);

     // Initialize DHT
    dht.begin();
}

void loop() {

    currentTemperature = dht.getTempCelcius();
    currentHumidity = dht.getHumidity();

    	//cast floats to string
	String temp(currentTemperature, 2);
	tempStr = temp;

	String hum(currentHumidity, 2);
	humStr = hum;

    // publish events
    publish("Temperature", currentTemperature);

    // I use Particle.publish for webhooks to thingsspeak - check Particle docs - webhooks
    Particle.publish("temp",tempStr,PRIVATE);
    delay(DELAY_PUBLISH);

    // Same here - webhooks
    publish("Humidity", currentHumidity);
    Particle.publish("hum",humStr,PRIVATE);

    delay(DELAY_LOOP);
}
