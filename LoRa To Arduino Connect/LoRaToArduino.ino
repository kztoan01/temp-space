#define MAX 100
#include <SPI.h>
#include <LoRa.h>
#include <stdio.h>
int temperature_pin = 0;
void setup() {
  pinMode(temperature_pin, INPUT);
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("LoRa Sender");
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  LoRa.setTxPower(20);
}

void loop() {
  //Read LM35
  int rawvoltage = analogRead(temperature_pin);

  float millivolts = (rawvoltage / 1024.0) * 5000;
  float celsius = millivolts / 10;

  Serial.print(millivolts);
  Serial.println(" millivolts");

  Serial.print(celsius);
  Serial.println(" degrees Celsius");

  Serial.print((celsius * 9) / 5 + 32);
  Serial.println(" degrees Fahrenheit");
  delay(1000);
char str[20];
  sprintf(str, "%f", celsius);
  // Gửi thông tin qua LoRa
  String message = createMessage(celsius);
  sendMessage(message,str);
  delay(1000);  // Delay 1 giây trước khi gửi tin nhắn tiếp theo
}


String createMessage(float celsius) {
  String message = "Motor: ";
  if (celsius > 25) {
    message += "ON, ";
  } else {
    message += "OFF, ";
  }
  message += "Temps123: ";
  message += celsius;

  return message;
}

void sendMessage(String message, String celsius) {
  String encodedMessage = hexEncode(message);
  String encodedMessage2 = hexEncode(celsius);
  LoRa.beginPacket();
  LoRa.print(encodedMessage+encodedMessage2);
  LoRa.endPacket();
}

String hexEncode(String input) {
  String encodedString = "";
  char hex[17] = "0123456789ABCDEF";
  for (unsigned int i = 0; i < input.length(); i++) {
    byte c = input.charAt(i);
    encodedString += hex[c >> 4];
    encodedString += hex[c & 0xF];
  }
  return encodedString;
}