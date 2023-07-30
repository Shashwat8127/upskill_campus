#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Define the pins for the relay module
#define RELAY_PIN_1 2
#define RELAY_PIN_2 3
#define RELAY_PIN_3 4

// Define the DHT sensor pin
#define DHT_PIN 5

// Define the MAC and IP addresses
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
IPAddress server(192, 168, 1, 100);

// Initialize the Ethernet client
EthernetClient client;

// Create a DHT object
DHT dht(DHT_PIN, DHT11);

// Function to send HTTP POST request
void sendRequest(String request) {
  if (client.connect(server, 80)) {
    client.println(request);
    client.println();
    client.stop();
  }
}

// Function to read temperature and humidity from DHT sensor
String readSensorData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    return "Error";
  }

  String data = "temperature=" + String(temperature) + "&humidity=" + String(humidity);
  return data;
}

// Function to control the appliances
void controlAppliances(String command) {
  if (command == "light_on") {
    digitalWrite(RELAY_PIN_1, HIGH);
  } else if (command == "light_off") {
    digitalWrite(RELAY_PIN_1, LOW);
  } else if (command == "fan_on") {
    digitalWrite(RELAY_PIN_2, HIGH);
  } else if (command == "fan_off") {
    digitalWrite(RELAY_PIN_2, LOW);
  } else if (command == "lock") {
    digitalWrite(RELAY_PIN_3, HIGH);
  } else if (command == "unlock") {
    digitalWrite(RELAY_PIN_3, LOW);
  }
}

void setup() {
  // Initialize the relay pins as outputs
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);

  // Start the Ethernet connection
  Ethernet.begin(mac, ip);

  // Initialize the DHT sensor
  dht.begin();
}

void loop() {
  // Check if the Ethernet client is connected
  if (client.connected()) {
    // Read the command from the server
    String command = client.readStringUntil('\n');
    command.trim();

    // Control the appliances based on the command
    controlAppliances(command);
  } else {
    // Generate the data from the DHT sensor
    String data = readSensorData();

    // Create the HTTP POST request
    String request = "POST /data HTTP/1.1\n";
    request += "Host: 192.168.1.100\n";
    request += "Content-Type: application/x-www-form-urlencoded\n";
    request += "Content-Length: " + String(data.length()) + "\n\n";
    request += data;

    // Send the request to the server
    sendRequest(request);
  }

  // Wait for a few seconds before the next iteration
  delay(5000);
}