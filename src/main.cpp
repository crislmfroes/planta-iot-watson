#include "ThingSpeak.h"
#include "secrets.h"
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define sensor A0
const char* BROKER_MQTT = "iot.eclipse.org";
const int BROKER_PORT = 1883;
#define INTERVAL_THINGSPEAK 20000
#define INTERVAL_MQTT 10000

long lastConnectionTime;
long lastMQTTSendTime;

char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password
int keyIndex = 0;          // your network key Index number (needed only for WEP)
WiFiClient client;
WiFiClient clientMQTT;
PubSubClient MQTT(clientMQTT);

unsigned long myChannelNumber = SECRET_CH_ID;
const char *myWriteAPIKey = SECRET_WRITE_APIKEY;

float readHumidity() {
  int read_value = analogRead(sensor);
  float percentual = map(1024 - read_value, 0, 1024, 0, 100);
  return percentual;
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.println(topic);  
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(SUBSCRIBE_TOPIC);
    } else {
      Serial.println("Falha ao conectar com broker MQTT. Tentando novamente em 2 segundos");
      delay(2000);
    }
  }
}

void reconnectWiFi() {
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
    Serial.print("IP Adress: ");
    Serial.println(WiFi.localIP());
  }
}

void initWiFi() {
  Serial.print("Conectando-se na rede: ");
  Serial.println(SECRET_SSID);
  reconnectWiFi();
}

void verifyWiFiAndMQTT() {
  if (!MQTT.connected()) {
    reconnectMQTT();
  }
  reconnectWiFi();
}

void setupOTA() {
  ArduinoOTA.onStart([]() {
    Serial.println("Inicio...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("nFim!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progresso: %u%%r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro [%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Autenticacao Falhou");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Falha no Inicio");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Falha na Conexao");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Falha na Recepcao");
    else if (error == OTA_END_ERROR)
      Serial.println("Falha no Fim");
  });
  ArduinoOTA.begin();
}

void setup()
{
  Serial.begin(9600); // Initialize serial
  pinMode(sensor, INPUT);
  lastConnectionTime = 0;
  lastMQTTSendTime = 0;
  initWiFi();
  initMQTT();
  ThingSpeak.begin(client);
  setupOTA();
  Serial.println("Planta Wilson IoT");
}

void loop()
{  

  ArduinoOTA.handle();

  verifyWiFiAndMQTT();
  
  int humidity = readHumidity();
  char MsgUmidityMQTT[50];

  if (millis() - lastConnectionTime > INTERVAL_THINGSPEAK) {
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeField(myChannelNumber, 1, humidity, myWriteAPIKey);
    if (x == 200)
    {
      Serial.println("Channel update successful.");
    }
    else
    {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastConnectionTime = millis();
  }

  if (millis() - lastMQTTSendTime > INTERVAL_MQTT) {
    sprintf(MsgUmidityMQTT, "%d", humidity);
    MQTT.publish(PUBLISH_TOPIC, MsgUmidityMQTT);
    lastMQTTSendTime = millis();
  }

  delay(1000); // Wait 2 seconds to update again
}