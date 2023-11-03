#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

unsigned long period = 2000; 
unsigned long last_time = 0; 

const char* ssid = "Rinwifi-2.4G";
const char* password = "Rin55555";

const char* mqttServer = "mqtt-dashboard.com";
const int mqttPort = 1883;

#define DHTPIN 32           
#define DHTTYPE DHT11      

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
}

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(client.state());
      Serial.println("Retrying in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  if (millis() - last_time > period) {
    last_time = millis();
    if (!isnan(humidity) && !isnan(temperature)) {
      String temperatureData = String(temperature) ;
      String humidityData = String(humidity) ;
      
      client.publish("your/hivemq/temperature", temperatureData.c_str());
      
      client.publish("your/hivemq/humidity", humidityData.c_str());
    }
  }
}
