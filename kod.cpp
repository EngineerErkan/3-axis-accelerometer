//kütüphanler yüklendi
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

// üç eksenli değerler oluşturuldu
int x;
int y; 
int z; 

// değerler okunarak x, y ve z değerlerine atandı
x = Read(5); 
y = Read(4); 
z = Read(3); 

//değerler ekrana yazdırıldı
Serial.print("X = "); 
Serial.println(x);
Serial.print("Y = "); 
Serial.println(y);
Serial.print("Z = "); 
Serial.println(z);

// servis ve şifre değerleri ayarlandı
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

//mqtt adresi ayarlandı
const char* mqtt_server = "192.168.1.144";

//wifi class oluşturuldu
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//sensör wifi bağlantısı
void setup() {
  Serial.begin(115200);
  if (!bme.begin(0x76)) {
    Serial.println("sensör bulunamadı");
    while (1);
  }
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
//wifi kuruldu
void setup_wifi() {
  delay(10);
  Serial.print("Bağlanıyor... ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Bağlandı");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic,  unsigned int length) {
  Serial.print("Değer ");
  Serial.print(topic);

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "esp32/output") {
    Serial.print("Mesajı değiştirme ");
    if(messageTemp == "on"){
      Serial.println("on");
    }
    else if(messageTemp == "off"){
      Serial.println("off");
    }
  }
}

//tekrar bağlantısı class
void reconnect() {

  while (!client.connected()) {
    Serial.print("MQTT connection");

    if (client.connect("ESP8266Client")) {
      Serial.println("bağlandı");
      client.subscribe("esp32/output");
    } else {
      Serial.print("bağlanmadı, rc=");
      Serial.print(client.state());

    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    