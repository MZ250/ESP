#include <ESP8266WiFi.h>
#include "UbidotsMicroESP8266.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <elapsedMillis.h>
elapsedMillis timeElapsed;

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid = "VM0527624";   //TEO-927285    Salionas
const char* password = "fwkkvzXNc4gc";   //Kest-6-2

WiFiServer server(80);
int ledPin = 13;
int redLed = 5;
int count = 0;
unsigned int interval = 900000;    //15 min   900000
unsigned int connectionTimeout = 30000; //30 s

#define TOKEN  "nj2nvt1NvrqIWsyumwbLHW1QjzCUJz"  // Put here your Ubidots TOKEN
#define WIFISSID "VM0527624"
#define PASSWORD "fwkkvzXNc4gc"

Ubidots client1(TOKEN);




void setup() {
  Serial.begin(115200);
  //client1.wifiConnection(WIFISSID, PASSWORD);
  sensors.begin();
  delay(10);
  pinMode(ledPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(redLed, HIGH);
 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    if (timeElapsed > connectionTimeout){
      ESP.restart();
    }
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  server.begin();                    // Start the server
  Serial.println("Server started");

  Serial.println(WiFi.localIP());    // Print the IP address
}

void ledSending(){
  for (int i = 0; i < 15; i++){
    digitalWrite (ledPin, LOW);
    delay(60);
    digitalWrite (ledPin, HIGH);
    delay(60);
  }
}


int uploadData(int sensor0,int sensor1){

    
  if (sensor0 < 0){
    ESP.restart();
  }
  String katilas = String(sensor0);
  String ketursakis = String(sensor1);

  client1.add("Katilas", sensor0);
  client1.add("Keturšakis", sensor1);
  client1.sendAll(true);
  
  delay (100); 
  count++;
  timeElapsed = 0;
 
}

void resetModule(){
  if (count > 16){
      ESP.restart();               
  }
}

void loop() {

  sensors.requestTemperatures();
  float reading0 = sensors.getTempCByIndex(0);
  float reading1 = sensors.getTempCByIndex(1);
  if (reading0 < 0){
    ESP.restart();
  }
  
  
  WiFiClient client = server.available();
 
  if (!client) { 
    if (WiFi.status() != WL_CONNECTED){
      ESP.restart();               
    }
    
 if (timeElapsed > interval){
   sensors.requestTemperatures();
   uploadData(sensors.getTempCByIndex(0), sensors.getTempCByIndex(1));
  }

   
    Serial.println("!client");
    bool state = 0;
    digitalWrite(redLed, state);
    digitalWrite(ledPin, HIGH);
    //resetModule();
    return;
    state = !state;
    delay(20);
  }
  //resetModule();
  digitalWrite(redLed, LOW);
  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    Serial.println("Client Unavailable");
    //resetModule();
    delay(1);
    return;
  }

 Serial.println("Client is sending data");
          
          client.println("HTTP/1.1 200 OK");
          Serial.println("1");
          client.println("Content-Type: text/html");
          Serial.println("2");
          client.println("Connection: open");  // the connection will be closed after completion of the response
          Serial.println("3");
          client.println("Refresh: 1");  // refresh the page automatically every 5 sec
          Serial.println("4");
          client.println("");
          Serial.println("5");
          client.println("<!DOCTYPE HTML>");
          Serial.println("6");
          client.println("<html>");
          client.print(reading0,1);
          client.println("°"); 
          client.println("         ");
          client.print(reading1,1);
          client.println("°"); 
          client.println("");
          client.println("");
          Serial.println("Sensor data sent");
          client.println("</html>");
          delay(1);
    client.stop();
    Serial.println("client.stop()");
    Serial.println("Client disonnected");
  
    //resetModule();

  if (timeElapsed > interval){
    sensors.requestTemperatures();
    uploadData(sensors.getTempCByIndex(0), sensors.getTempCByIndex(1));
  }
  
ledSending(); 
}

