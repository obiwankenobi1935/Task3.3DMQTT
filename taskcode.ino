#include <WiFiNINA.h>         // Include the WiFiNINA library for Arduino boards with WiFiNINA module
#include <PubSubClient.h>     // Include the PubSubClient library for MQTT

// Replace these with your network and broker details
const char* ssid = "Airtel_k19vandana";        // Your Wi-Fi SSID
const char* password = "OmShiv193"; // Your Wi-Fi Password
const char* mqtt_server = "broker.emqx.io"; // EMQX broker address

WiFiClient espClient;
PubSubClient client(espClient);

const int trigPin = 2;
const int echoPin = 3;
const int ledPin = 13;

unsigned long lastMessageTime = 0;
const long messageInterval = 2000; // 2 seconds interval to avoid multiple rapid detections

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Ultrasonic measurement
  long duration;
  float distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2.0) * 0.0344;

  // Check if an object is within 10 cm
  if (distance <= 10.0) {  // Adjust distance threshold as needed
    unsigned long now = millis();
    if (now - lastMessageTime > messageInterval) {
      publishMessage();
      lastMessageTime = now;
    }
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  int status = WiFi.begin(ssid, password);
  
  // Wait for connection to the Wi-Fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ArduinoClient")) {
      Serial.println("connected");
      client.subscribe("SIT210/wave");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];  
  }
  Serial.println(message);
  blinkLED(3); // Blink LED 3 times
}

void publishMessage() {
  String message = "Kavish Choudhary";  // Publish your name
  if (client.publish("SIT210/wave", message.c_str())) {
    Serial.println("Message published");
  } else {
    Serial.println("Message publish failed");
  }
}

void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(250);
  }
}
