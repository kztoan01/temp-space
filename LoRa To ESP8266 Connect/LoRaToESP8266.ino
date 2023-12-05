#include <SPI.h>
#include <LoRa.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <BlynkSimpleEsp8266.h>
//define devices from template on Blynk
//Define LoRa
#define LORA_SS_PIN 15
#define LORA_RESET_PIN 16
#define LORA_DIO0_PIN 2
//Wifi and Password
const char* ssid = "wifiname";
const char* password = "wifipassword";
char auth[] = "authtokenonblynk";
byte MasterNode = 0xFF;
byte Node1 = 0xBB;
String SenderNode = "";
String outgoing;    // outgoing message
byte msgCount = 0;  // count of outgoing messages
String incoming = "";
float ctemp;
float ftemp;
void setup() {
  Serial.begin(9600);
  delay(2000);
  //Connect to Wifi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Kết nối WiFi...");
  }

  Serial.println("WiFi đã kết nối!");
  Serial.print("Địa chỉ IP: ");
  Serial.println(WiFi.localIP());

  LoRa.setPins(LORA_SS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);

  if (!LoRa.begin(433E6)) {
    Serial.println("Khởi tạo LoRa thất bại. Vui lòng kiểm tra kết nối.");
    while (1)
      ; // check if start LoRa failed, do nothing
  }

  Serial.println("LoRa đã kết nối!");
  Blynk.begin(auth, ssid, password);
}

void loop() {
  Blynk.run();
  Blynk.virtualWrite(V1, 12.00);
  Blynk.virtualWrite(V2, 60.00);
  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;  // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();  // recipient address
  byte sender = LoRa.read();    // sender address
  if (sender == 0XBB)
    SenderNode = "Node1:";
  byte incomingMsgId = LoRa.read();   // incoming msg ID
  byte incomingLength = LoRa.read();  // incoming msg length


  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {  // check length for error
    //Serial.println("error: message length does not match length");
    ;
    return;  // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != Node1 && recipient != MasterNode) {
    // Serial.println("This message is not for me.");
    ;
    return;  // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  //Serial.println("Received from: 0x" + String(sender, HEX));
  //Serial.println("Sent to: 0x" + String(recipient, HEX));
  //Serial.println("Message ID: " + String(incomingMsgId));
  // Serial.println("Message length: " + String(incomingLength));
  // Serial.println("Message: " + incoming);
  //Serial.println("RSSI: " + String(LoRa.packetRssi()));
  // Serial.println("Snr: " + String(LoRa.packetSnr()));
  // Serial.println();


  String q = getValue(incoming, ',', 0);

  String r = getValue(incoming, ',', 1);

  ctemp = q.toFloat();
  Serial.print(ctemp);
  Serial.println(" C");
  ftemp = r.toFloat();
  Serial.print(ftemp);
  Serial.println(" F");
  Blynk.virtualWrite(V1, 12.00);
  Blynk.virtualWrite(V2, 60.00);
  if (ctemp >= 45) {
    Serial.println("To hot");
  }
  if (ctemp < 45) {

    Serial.println("Normal");
  }
  //delay(1000);
  incoming = "";
}


String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}