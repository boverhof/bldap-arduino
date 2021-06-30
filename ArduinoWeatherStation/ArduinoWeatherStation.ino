/*
 * Example reads DHT22 on pin7 for temperature (C) and humidity (%)
 * Sends the data off to the configured HTTP_SERVER
 */

#include <DHT.h>
#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h" 

//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino


//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

int max_loop = 10;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)
char user_id[] = USER_ID;

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(192,168,87,42);  // numeric IP
char server[] = HTTP_SERVER;    

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  String header1 = String("Host: ") + String(server);
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /arduino HTTP/1.1");
    client.println(header1);
    client.println("Connection: close");
    client.println();
  }
}
 

void client_disconnect() {
    // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}

void loop() {
  if (not client.available()) {
    Serial.println("==> Client Not Available");
    delay(1000);
    max_loop = max_loop - 1;
    if (max_loop > 0 ) {
      return;
    }
    max_loop = 10;
  } else {
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
    client.stop();
  }
  Serial.println("===> Reading DHT: ");
  read_dht();
  send_data_to_client();
  delay(2000);
  
}

void read_dht() {
    dht.begin();
    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp= dht.readTemperature();
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
}


void send_data_to_client() {
  String data = String("{\"temperature\":") + String(temp) + String(",\"deviceid\":\"") + String(user_id) + String("\",\"humidity\":") + String(hum) + String("}");
  String header1 = String("Host: ") + String(server);
  String header2 = String("Content-Length: ") + String(data.length());
  Serial.println("send data to client: " + data);
  if (client.connect(server, 80)) {
    Serial.println("JOSH connected to server");
    // Make a HTTP request:
    client.println("POST /arduino/temperature HTTP/1.1");
    client.println(header1);
    client.println(header2);
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.println(data);
  } else {
    Serial.println("FAILED connected to server");
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
