/* Includes for DHT11 sensor */
/* Requires the Arduino DHT Library */
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

/* Includes for LoRa Node */
#define DEBUG_MODE
#include "RAK811.h"
#include "SoftwareSerial.h"

/* DHT defines */
#define DHTPIN 2 // Feel free to pick a different pin
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);

/* LoRa Node defines */
#define TXpin 11
#define RXpin 10
#define PACKET_DELAY 60 // Seconds between packets
SoftwareSerial RAKSerial(RXpin, TXpin);
RAK811 RAKLoRa(RAKSerial, Serial);

/* Configure OTAA connection details */
String DevEui = "Please update";
String AppEui = "Please update";
String AppKey = "Please update";


//==================================================//


void setup() {
  Serial.begin(9600);
  /* Configure DHT11 */
  dht.begin();

  /* Configure LoRa Node */
  pinMode(12, OUTPUT); //Reset Pin
  digitalWrite(12, LOW); //Pull down Reset
  digitalWrite(12, HIGH); //Release reset

  RAKSerial.begin(9600);
  delay(100);
  while (!InitLoRaWAN());
  
}

void loop() {
  sensors_event_t event; // Create a variable to capture the sensor data
  dht.temperature().getEvent(&event); // Get temperature
  
  char temp_char[5]; // A temporary variable to store data packet.
  // We need 4 bytes to send the 16bit hex value plus 1 for the string null
  // character.
  int temp;
  
  /* This is an important step. Here we are taking the floating point value,
   * e.g. 25.32. Next we multiply by 10 to give 253.2 and then convert to an
   * integer 253.  * 
   */
  temp = (int)(event.temperature * 10); 
  sprintf(temp_char, "%04X", temp); // Convert to a 4 digit hex with leading zeros
  Serial.println(temp_char); // Send the value to the Arduino serial monitor
  
  int packetType = 0; // 0: Unconfirmed packets, 1: confirmed packets
  RAKLoRa.rk_setRate(5); // Use to change the DR (Data Rate) of the node. 0-5 
  RAKLoRa.rk_sendData(packetType, 1, temp_char); // Broadcast the data packet
  
  int i;
  for (i=0; i<PACKET_DELAY; i++)
  {
    delay(1000); // This 1 second delay is repeated PACKET_DELAY times.
  }  
} 

char InitLoRaWAN(void)
{
  RAKLoRa.rk_setConfig("adr", "on"); // set to "off" if your node is going to be moving
  RAKLoRa.rk_setWorkingMode(LoRaWAN);
  if (RAKLoRa.rk_initOTAA(DevEui, AppEui, AppKey))
  {
    Serial.println("OTAA init OK");
  }
  else
  {
    Serial.println("OTAA init FAILED");
  }
  
  delay(5000); 
  while (RAKLoRa.rk_joinLoRaNetwork(OTAA))
  {
    String ret = RAKLoRa.rk_recvData();
    Serial.println(ret);
    return 1;
  }
}
