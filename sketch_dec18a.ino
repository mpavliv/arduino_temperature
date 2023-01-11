#include <OneWire.h>
#include <DallasTemperature.h>
#include <TM1637.h>
#include <Ethernet.h>
#include <SPI.h>



////   ***********  Ethernet W5100
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// IPAddress ip = ( 10, 10, 0, 165 );
byte ip[] = { 10, 10, 0, 165 };
// IPAddress server(10,10,0,52);
// byte server[] = {185,65,246,123};
byte server[] = {185,65,246,123};
int port = 8080; 
IPAddress myDns(10, 10, 0, 1);
EthernetClient client;



// DS18B20
OneWire ds(7); 
DallasTemperature sensors(&ds);

DeviceAddress Thermometer1 = {0x28, 0xFF, 0x64, 0x0E, 0x71, 0xB7, 0x46, 0xE0 };

// TM1637
int CLK = 2;
int DIO = 3;
TM1637 tm(CLK, DIO);

void setup(void) {

Serial.begin(9600);
tm.init();
tm.setBrightness(2);
 while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to configure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

void loop(void) {
  int t1;
  Serial.println("Please wait..."); 
  sensors.requestTemperatures(); // Команда опроса температуры.
  Serial.print("на каміні=");
  t1=sensors.getTempC(Thermometer1);
  Serial.println(t1);
  String show = "    " + String(t1);  
  String s = show.substring(show.length() - 4, show.length());
  tm.display(s);
  Serial.println(show);
  httpRequest(t1);
  delay(4000);
}


void httpRequest(int t1) {
  String sTemperature = String(t1);
  String data = "{\"point\" : \"1\",\"datatime\": \"0\",\"value\": \"" + sTemperature + "\"}";
  if (client.connect(server, port)) {

    Serial.println("connecting...");
        
    delay(1000);
    Serial.println("Sending to Server: ");                    
    client.println("POST /temperature HTTP/1.1");
    client.println("Host: 10.10.0.51");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
    client.println();

    Serial.println(data);    
    delay(1000);   
    client.stop();
  } else {

    Serial.println("connection failed");

  }
}