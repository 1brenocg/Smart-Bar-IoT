// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <BlynkSimpleEsp8266.h>
#include <Ultrasonic.h>

#define TRIG_PIN D5
#define ECHO_PIN D6
#define DHTPIN D7 // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE DHT11 // DHT 11
// #define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

Ultrasonic us(TRIG_PIN, ECHO_PIN);
float dt;
int startTime;

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

const int sensorPin = A0; //sensor pin connected to analog pin A0
float liquid_level;
int liquid_percentage;
int top_level = 665;  //Maximum water level
int bottom_level = 0; //Minimum water level

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "4f12bf3c5f0646f4b44eb07c52231ebe";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "sergio";
char pass[] = "12345678";

int contPessoas = 0;

void setup()
{
  Serial.begin(9600);
  // Initialize device.
  startTime = millis();
  dht.begin();
  Blynk.begin(auth, ssid, pass);
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("°C"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("°C"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("%"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("%"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
  pinMode(D1, OUTPUT);
  digitalWrite(D1, HIGH);

  Blynk.begin(auth, ssid, pass);
}

void loop(){

  Blynk.run();

  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.

  Serial.println("##########################################"); 

  //------------------- Módulo do Sensor de Temp e Humidade --------------------------------------//
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println(F("Error reading temperature!"));
  }
  else
  {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    Blynk.virtualWrite(V3, event.temperature);

    if(digitalRead(D2)){
      digitalWrite(D1, LOW);
      Blynk.virtualWrite(V5, HIGH);
    }

    if ((event.temperature) > 33){
      digitalWrite(D1, LOW);
      Blynk.virtualWrite(V5, HIGH);
      // Blynk.virtualWrite(D1, LOW );
      Blynk.notify("Temperatura muita alta!");
    }
    else if ((event.temperature) < 33 && (!digitalRead(D2))){
      digitalWrite(D1, HIGH);
      Blynk.virtualWrite(V5, LOW);
    }
    else if(!digitalRead(D2)){
      digitalWrite(D1, HIGH);
      Blynk.virtualWrite(V5, LOW);
    }
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println(F("Error reading humidity!"));
  }
  else
  {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Blynk.virtualWrite(V2, event.relative_humidity);
    // digitalWrite(D1, LOW);
    Serial.println(F("%"));
  }
  //------------------- Módulo do Sensor de Temp e Humidade --------------------------------------//

  //------------------- Módulo do Sensor de Distancia --------------------------------------//
  us.measure();

  Serial.print("Distancia = ");
  Serial.print(us.get_cm()); //mostra no monitor serial a distancia
  // Blynk.virtualWrite(V4, us.get_cm());
  Serial.print("\nQuantidade de pessoas: ");
  Serial.println(contPessoas);  //quantidade de pessoas
  if (us.get_cm() < 15)
  {
    Serial.println("Uma pessoa entrou!"); //mostra no monitor serial que uma pessoa entrou
    Blynk.notify("Uma pessoa entrou!");   //notifica quando uma pessoa entra
    contPessoas = contPessoas + 1;
    Serial.println(contPessoas);         //mostra no monitor serial a quantidade de pessoas
    Blynk.virtualWrite(V1, contPessoas); //mostra a quantidade de pessoas no blynk
  }
   
  //------------------- Módulo do Sensor de Distancia --------------------------------------//

  //------------------- Módulo do Sensor de Água --------------------------------------//
  liquid_level = analogRead(sensorPin);
  liquid_percentage = ((liquid_level - bottom_level) / top_level) * 100; //Percentage of water in the container
  Serial.print("\nNivel de Liquido: ");
  Serial.println(liquid_level); //This will print the liquid level in the monitor
  Serial.print("\nPorcentagem de Liquido: ");
  Serial.println(liquid_percentage); //This will print the percentage of liquid in the monitor
  Serial.println("");
  if (liquid_level <= 500)
  {
    // Blynk.notify("Mesa 1 Acabou a cerveja");
  }
  //------------------- Módulo do Sensor de Água --------------------------------------//
  Serial.println("##########################################\n");
  delay(600);
}
