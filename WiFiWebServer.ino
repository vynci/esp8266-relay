#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "your-wifi-ssid";
const char* password = "your-wifi-password";

char* topic = "device/control";
char* topicPublish = "device/sensor";
char* server = "85.119.83.194"; //ip address of http://test.mosquitto.org/


WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

char message_buff[100];

void callback(char* topic, byte* payload, unsigned int length) {
  int i = 0;

  Serial.println("Message arrived:  topic: " + String(topic));
  Serial.println("Length: " + String(length,DEC));

  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';

  String msgString = String(message_buff);

  Serial.println("Payload: " + msgString);
  
  if(msgString == "dev1-on"){
    Serial.println("Device 1 on");
    digitalWrite(2, 1);
  } else if(msgString == "dev1-off"){
    Serial.println("Device 1 off");
    digitalWrite(2, 0);    
  } else if(msgString == "dev2-on"){
    Serial.println("Device 2 on");
    digitalWrite(0, 1);
  } else if(msgString == "dev2-off"){
    Serial.println("Device 2 off");    
    digitalWrite(0, 0);
  }
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

void setup() {
  
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Generate client name based on MAC address and last 8 bits of microsecond counter
  String clientName;
  clientName += "esp8266-";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);

  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);

  if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Topic is: ");
    Serial.println(topic);
    if (client.subscribe(topic)){
      Serial.println("Successfully subscribed");
    }
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }
}

void loop() {
  client.loop();
}
