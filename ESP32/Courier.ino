#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 5
#define RST_PIN 0

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

const char* ssid ="KL";
const char* password =  "YeahThatFits";
const char* mqttServer = "dev.rightech.io";
const int mqttPort = 1883;
const char* mqttUser = "KirCoff";
const char* mqttPassword = "YeahThatFits";
const char* ClientID = "CourierTest";

Servo servo1, servo2, servo3, servo4, servo5, servo6, servo7, servo8;

int lastMsg = 0;
char msg[3];

char cont1[12] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
char cont2[12] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
char cont3[12] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
char cont4[12] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
char cont5[12] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
char cont6[12] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
char cont7[12] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
char cont8[12] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
char op[12] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

byte res[4] = {0,0,0,0};

const int buttonPin = 21;
const int ledRed = 17;
const int ledYellow = 16;
const int ledGreen = 4;
 
WiFiClient espClient;
PubSubClient client(espClient);

void convertToInt(char* orig) {
    int i = 0;
    for (byte k = 0; k<4; k++){
      res[k] = 0;
    }
    for (byte j = 0; j < 11; j++) {
        switch (orig[j])
        {
        case '0': res[i] = res[i] * 16 + 0;
            break;
        case '1': res[i] = res[i] * 16 + 1;
            break;
        case '2': res[i] = res[i] * 16 + 2;
            break;
        case '3': res[i] = res[i] * 16 + 3;
            break;
        case '4': res[i] = res[i] * 16 + 4;
            break;
        case '5': res[i] = res[i] * 16 + 5;
            break;
        case '6': res[i] = res[i] * 16 + 6;
            break;
        case '7': res[i] = res[i] * 16 + 7;
            break;
        case '8': res[i] = res[i] * 16 + 8;
            break;
        case '9': res[i] = res[i] * 16 + 9;
            break;
        case 'A': res[i] = res[i] * 16 + 10;
            break;
        case 'B': res[i] = res[i] * 16 + 11;
            break;
        case 'C': res[i] = res[i] * 16 + 12;
            break;
        case 'D': res[i] = res[i] * 16 + 13;
            break;
        case 'E': res[i] = res[i] * 16 + 14;
            break;
        case 'F': res[i] = res[i] * 16 + 15;
            break;
        case ' ': i++;
            break;
        default:
            break;
        }
    }
}

void mqttconnect() {
  digitalWrite(ledGreen, LOW);
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    digitalWrite(ledYellow, HIGH);
    if (client.connect(ClientID, mqttUser, mqttPassword)) {
      Serial.println("connected");

      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, HIGH);

      client.subscribe("to/client/cont1");
      client.subscribe("to/client/cont2");
      client.subscribe("to/client/cont3");
      client.subscribe("to/client/cont4");
      client.subscribe("to/client/cont5");
      client.subscribe("to/client/cont6");
      client.subscribe("to/client/cont7");
      client.subscribe("to/client/cont8");
      client.subscribe("to/client/op");
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");

      delay(5000);
    }
  }
}
 
void callback(char*topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived in topic: ");
  //Serial.println(topic);
  //Serial.print("Message: ");
  for (byte i = 2; i < length; i++) {
    //Serial.print((char)payload[i]);
    if (strcmp(topic,"to/client/cont1")==0){
      cont1[i-2] = (char)payload[i];
    }
    else if (strcmp(topic,"to/client/cont2")==0){
      cont2[i-2] = (char)payload[i];
    }
    else if (strcmp(topic,"to/client/cont3")==0){
      cont3[i-2] = (char)payload[i];
    }
    else if (strcmp(topic,"to/client/cont4")==0){
      cont4[i-2] = (char)payload[i];
    }
    else if (strcmp(topic,"to/client/cont5")==0){
      cont5[i-2] = (char)payload[i];
    }
    else if (strcmp(topic,"to/client/cont6")==0){
      cont6[i-2] = (char)payload[i];
    }
    else if (strcmp(topic,"to/client/cont7")==0){
      cont7[i-2] = (char)payload[i];
    }
    else if (strcmp(topic,"to/client/cont8")==0){
      cont8[i-2] = (char)payload[i];
    }
    else if (strcmp(topic,"to/client/op")==0){
      op[i-2] = (char)payload[i];
    }
  }
}

void WiFiConnect(){
  bool led = false;
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status()!= WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(led == false){
      digitalWrite(ledRed, HIGH);
      led = true;
    }
    else{
      digitalWrite(ledRed, LOW);
      led = false;
    }
  }
  Serial.println(" ");
  Serial.println("Connected to the WiFi network");
  led = false;
  digitalWrite(ledRed, LOW);
}


void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(buttonPin, INPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  servo1.attach(13);
  servo2.attach(12);
  servo3.attach(14);
  servo4.attach(27);
  servo5.attach(26);
  servo6.attach(25);
  servo7.attach(33);
  servo8.attach(32);

  servo1.write(0);
  delay(500);
  servo2.write(0);
  delay(500);
  servo3.write(0);
  delay(500);
  servo4.write(0);
  delay(500);
  servo5.write(0);
  delay(500);
  servo6.write(0);
  delay(500);
  servo7.write(0);
  delay(500);
  servo8.write(0);

  digitalWrite(ledRed, LOW);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledGreen, LOW);

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  WiFiConnect();

  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}
 
void loop() {
  long now = millis();

  if(digitalRead(buttonPin) == HIGH){
    ESP.restart();
  }

  if (!client.connected()) {
    mqttconnect();
  }
  client.loop();

  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("Card UID: "));
  checkCard(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

  rfid.PICC_HaltA();

  rfid.PCD_StopCrypto1();
}


void checkCard(byte *buffer, byte bufferSize) {
  
  byte equal[9] = {0,0,0,0,0,0,0,0,0};

  convertToInt(cont1);
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    if(buffer[i] == res[i])
      equal[1]++;
  }
  convertToInt(cont2);
  for (byte i = 0; i < bufferSize; i++) {
    if(buffer[i] == res[i])
      equal[2]++;
  }
  convertToInt(cont3);
  for (byte i = 0; i < bufferSize; i++) {
    if(buffer[i] == res[i])
      equal[3]++;
  }
  convertToInt(cont4);
  for (byte i = 0; i < bufferSize; i++) {
    if(buffer[i] == res[i])
      equal[4]++;
  }
  convertToInt(cont5);
  for (byte i = 0; i < bufferSize; i++) {
    if(buffer[i] == res[i])
      equal[5]++;
  }
  convertToInt(cont6);
  for (byte i = 0; i < bufferSize; i++) {
    if(buffer[i] == res[i])
      equal[6]++;
  }
  convertToInt(cont7);
  for (byte i = 0; i < bufferSize; i++) {
    if(buffer[i] == res[i])
      equal[7]++;
  }
  convertToInt(cont8);
  for (byte i = 0; i < bufferSize; i++) {
    if(buffer[i] == res[i])
      equal[8]++;
  }
  convertToInt(op);
  for (byte i = 0; i < bufferSize; i++) {
    if(buffer[i] == res[i])
      equal[0]++;
  }
  
  if(equal[1] == bufferSize){
    Serial.print(F("\nContainer 1 is opened\n"));
    for(int i = 0; i<11; i++){
    }
    snprintf (msg, 4, "%s", "2|0");
    client.publish("to/client/cont1", msg, true);
    servo1.write(90);
    delay(500);
  }
  if(equal[2] == bufferSize){
    Serial.print(F("\nContainer 2 is opened\n"));
    for(int i = 0; i<11; i++){
    }
    snprintf (msg, 4, "%s", "2|0");
    client.publish("to/client/cont2", msg, true);
    servo2.write(90);
    delay(500);
  }
  if(equal[3] == bufferSize){
    Serial.print(F("\nContainer 3 is opened\n"));
    for(int i = 0; i<11; i++){
    }
    snprintf (msg, 4, "%s", "2|0");
    client.publish("to/client/cont3", msg, true);
    servo3.write(90);
    delay(500);
  }
  if(equal[4] == bufferSize){
    Serial.print(F("\nContainer 4 is opened\n"));
    for(int i = 0; i<11; i++){
    }
    snprintf (msg, 4, "%s", "2|0");
    client.publish("to/client/cont4", msg, true);
    servo4.write(90);
    delay(500);
  }
  if(equal[5] == bufferSize){
    Serial.print(F("\nContainer 5 is opened\n"));
    for(int i = 0; i<11; i++){
    }
    snprintf (msg, 4, "%s", "2|0");
    client.publish("to/client/cont5", msg, true);
    servo5.write(90);
    delay(500);
  }
  if(equal[6] == bufferSize){
    Serial.print(F("\nContainer 6 is opened\n"));
    for(int i = 0; i<11; i++){
    }
    snprintf (msg, 4, "%s", "2|0");
    client.publish("to/client/cont6", msg, true);
    servo6.write(90);
    delay(500);
  }
  if(equal[7] == bufferSize){
    Serial.print(F("\nContainer 7 is opened\n"));
    for(int i = 0; i<11; i++){
    }
    snprintf (msg, 4, "%s", "2|0");
    client.publish("to/client/cont7", msg, true);
    servo7.write(90);
    delay(500);
  }
  if(equal[8] == bufferSize){
    Serial.print(F("\nContainer 8 is opened\n"));
    for(int i = 0; i<11; i++){
    }
    snprintf (msg, 4, "%s", "2|0");
    client.publish("to/client/cont8", msg, true);
    servo8.write(90);
    delay(500);
  }
  if(equal[0] == bufferSize){
    Serial.print(F("\nAll containers are opened\n"));
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledYellow, HIGH);
    servo1.write(90);
    delay(500);
    servo2.write(90);
    delay(500);
    servo3.write(90);
    delay(500);
    servo4.write(90);
    delay(500);
    servo5.write(90);
    delay(500);
    servo6.write(90);
    delay(500);
    servo7.write(90);
    delay(500);
    servo8.write(90);
    delay(500);
    delay(10000);
  }
  if(equal[1] == bufferSize || equal[2] == bufferSize || equal[3] == bufferSize ||
     equal[4] == bufferSize || equal[5] == bufferSize || equal[6] == bufferSize ||
     equal[7] == bufferSize || equal[8] == bufferSize || equal[0] == bufferSize){
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledYellow, HIGH);
      delay(5000);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, HIGH);
      servo1.write(0);
      delay(500);
      servo2.write(0);
      delay(500);
      servo3.write(0);
      delay(500);
      servo4.write(0);
      delay(500);
      servo5.write(0);
      delay(500);
      servo6.write(0);
      delay(500);
      servo7.write(0);
      delay(500);
      servo8.write(0);
      delay(500);
   }
}
