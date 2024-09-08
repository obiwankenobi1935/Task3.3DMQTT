#include <WiFiNINA.h>         
#include <PubSubClient.h>     


const char* ssid = "Airtel_k19vandana";
const char* password = "OmShiv193"; 
const char* mqtt_server = "broker.emqx.io";

WiFiClient espClient;
PubSubClient client(espClient);

const int trigPin = 2;
const int echoPin = 3;
const int ledPin = 13;

unsigned long lastMessageTime = 0;
const long messageInterval = 2000; 

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

  
  long duration;
  float distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2.0) * 0.0344;

 
  if (distance <= 5.0) {
    unsigned long now = millis();
    if (now - lastMessageTime > messageInterval) {
      publishPatMessage();
      blinkPatLED();
      lastMessageTime = now;
    }
  }
 
  else if (distance <= 10.0) {
    unsigned long now = millis();
    if (now - lastMessageTime > messageInterval) {
      publishWaveMessage();
      blinkWaveLED();
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
  
  \
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

  if (String(topic) == "SIT210/wave") {
    Serial.println("Wave or pat detected.");
  }
}


void publishPatMessage() {
  String message = "Pat detected by Kavish Choudhary";  // Custom message for pat
  if (client.publish("SIT210/wave", message.c_str())) {
    Serial.println("Pat message published");
  } else {
    Serial.println("Pat message publish failed");
  }
}


void publishWaveMessage() {
  String message = "Wave detected by Kavish Choudhary";  // Custom message for wave
  if (client.publish("SIT210/wave", message.c_str())) {
    Serial.println("Wave message published");
  } else {
    Serial.println("Wave message publish failed");
  }
}


void blinkPatLED() {
  for (int i = 0; i < 5; i++) {  // 5 short blinks
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}


void blinkWaveLED() {
  for (int i = 0; i < 3; i++) {  // 3 longer blinks
    digitalWrite(ledPin, HIGH);
    delay(300);
    digitalWrite(ledPin, LOW);
    delay(300);
  }
}
