/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
//#define BLYNK_TEMPLATE_ID   "YourTemplateID"


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include "DHT.h"
DHT dht;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "NmNRkQ8YS-ZZWw5Gzw_rsrIs6ILL0xtD";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "woodhouse_2.4G";
char pass[] = "0840850744";


#define USE_AVG
 
// Arduino pin numbers.
const int sharpLEDPin = D2; // Arduino digital pin 7 connect to sensor LED.
const int sharpVoPin = A0; // Arduino analog pin 5 connect to sensor Vo.
 
// For averaging last N raw voltage readings.
#ifdef USE_AVG
#define N 100
static unsigned long VoRawTotal = 0;
static int VoRawCount = 0;
#endif // USE_AVG
 
// Set the typical output voltage in Volts when there is zero dust. 
static float Voc = 0.6;
 
// Use the typical sensitivity in units of V per 100ug/m3.
const float K = 0.5;
/////////////////////////////////////////////////////////////////////////////
 
// Helper functions to print a data value to the serial monitor.
void printValue(String text, unsigned int value, bool isLast = false) {
Serial.print(text);
Serial.print("=");
Serial.print(value);
if (!isLast) {
Serial.print(", ");
}
}
void printFValue(String text, float value, String units, bool isLast = false) {
Serial.print(text);
Serial.print("=");
Serial.print(value);
Serial.print(units);
if (!isLast) {
Serial.print(", ");
}
} 
/////////////////////////////////////////////////////////////////////////////
float dustDensity;

int Relay = D3; 

int HumSET = 80;
int Pm25SET = 300;

int button1;

BLYNK_WRITE(V0) {
  button1 = param.asInt();
}

WidgetLED led1(V4);
//-------------------------------------------------------------------------------------
void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);

  pinMode(Relay, OUTPUT);
  pinMode(sharpLEDPin, OUTPUT);
  
   Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)");
  dht.setup(5); // data pin 5

}

//-------------------------------------------------------------------------------------
void loop()
{ 
// Turn on the dust sensor LED by setting digital pin LOW.
digitalWrite(sharpLEDPin, LOW);
 
// Wait 0.28ms before taking a reading of the output voltage as per spec.
delayMicroseconds(280);
 
// Record the output voltage. This operation takes around 100 microseconds.
int VoRaw = analogRead(sharpVoPin);
 
// Turn the dust sensor LED off by setting digital pin HIGH.
digitalWrite(sharpLEDPin, HIGH);
 
// Wait for remainder of the 10ms cycle = 10000 - 280 - 100 microseconds.
delayMicroseconds(9620);
 
// Print raw voltage value (number from 0 to 1023).
#ifdef PRINT_RAW_DATA
printValue("VoRaw", VoRaw, true);
Serial.println("");
#endif // PRINT_RAW_DATA
 
// Use averaging if needed.
float Vo = VoRaw;
#ifdef USE_AVG
VoRawTotal += VoRaw;
VoRawCount++;
if ( VoRawCount >= N ) {
Vo = 1.0 * VoRawTotal / N;
VoRawCount = 0;
VoRawTotal = 0;
} else {
return;
}
#endif // USE_AVG
 
// Compute the output voltage in Volts.
Vo = Vo / 1024.0 * 5.0;
printFValue("Vo", Vo, "V");
 
// Convert to Dust Density in units of ug/m3.
float dV = Vo - Voc;
if ( dV < 0 ) {
dV = 0;
Voc = Vo;
}
 dustDensity = dV / K * 100.0;
printFValue("DustDensity", dustDensity, "ug/m3", true);
Serial.println("");
 Blynk.virtualWrite(V3, dustDensity); 


if(dustDensity>=Pm25SET&&button1==0)
{
   {
  led1.off();
  digitalWrite(Relay, HIGH);
}
  led1.on();
  digitalWrite(Relay, LOW);
 {
  led1.off();
  digitalWrite(Relay, HIGH);
}
{
  led1.on();
  delay(40000);
}
}

if(dustDensity<Pm25SET&&button1==0)
{
  led1.off();
  digitalWrite(Relay, LOW);
}
//-------------------------------------------------------------------------------------



  
  delay(dht.getMinimumSamplingPeriod());
  float humidity = dht.getHumidity(); // ดึงค่าความชื้น
  float temperature = dht.getTemperature(); // ดึงค่าอุณหภูมิ
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.println(dht.toFahrenheit(temperature), 1);
  Blynk.run();
 // delay(100);
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity); 
  Blynk.virtualWrite(V3, dustDensity); 
if(humidity>=HumSET&&button1==1)
{
   {
  led1.off();
  digitalWrite(Relay, HIGH);
}
  led1.on();
  digitalWrite(Relay, LOW);
 {
  led1.off();
  digitalWrite(Relay, HIGH);
}
{
  led1.on();
  delay(20000);
}
}
if(humidity<HumSET&&button1==1)
{
  led1.off();
  digitalWrite(Relay, LOW);
  
}
//-------------------------------------------------------------------------------------



  Blynk.run();
}

//-----------------------------------------END--------------------------------------------
