
// Programme TWO - PING
#include <RH_RF95.h>
#define RF95_CS 10
#define RF95_INT 2
#define RF95_RST 9
#define RF95_FREQ 915.0

#define LED 4

RH_RF95 rf95(RF95_CS, RF95_INT);
uint8_t data[RH_RF95_MAX_MESSAGE_LEN];

bool sending = false;
String clientId = "Client #1";
int timeOut = 0;

void setup()
{
  timeOut = 0;
  sending = false;
  Serial.begin(9600);
  pinMode(RF95_RST, OUTPUT);
  digitalWrite(RF95_RST, HIGH);
  delay(10);
  digitalWrite(RF95_RST, LOW);
  delay(10);
  digitalWrite(RF95_RST, HIGH);
  delay(10);
  rf95.init();
  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23);
  // rf95.setModemConfig(RH_RF95::ModemConfigChoice::Bw31_25Cr48Sf512);

  // RH_RF95::ModemConfig modem_config = {
  //   0x78, // Reg 0x1D: BW=125kHz, Coding=4/8, Header=explicit
  //   0xc4, // Reg 0x1E: Spread=4096chips/symbol, CRC=enable
  //   0x0c  // Reg 0x26: LowDataRate=On, Agc=On
  // };
  // rf95.setModemRegisters(&modem_config);
}

void blinkLED(){
  digitalWrite(LED, LOW);
  Serial.println("slfjhvi");
  delay(500);
  for (uint8_t i = 0; i < 4; i++){
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, HIGH);
    delay(100);
  }
  delay(500);
}

void loop(){
  // if this device recives the request for-
  // -"Client #1", then it will run all the following code:

  if (!sending){
    if (!rf95.available()){
      return;
    }
    uint8_t len  = sizeof(data);
    if (!rf95.recv(data, &len)){
      return;
    }

    String recievedMessage = (char*)data;

    recievedMessage.trim();
    Serial.println(recievedMessage);
    Serial.println(recievedMessage == clientId);

    if (recievedMessage == clientId){
      sending = true;
      Serial.println("Connection Established");
    }
  }
  else{
    if(Serial.available() > 0){
      timeOut = 0;
      delay(100);
      String data = Serial.readString();
      Serial.println(data);

      char msg[300];
      data.toCharArray(msg, data.length() + 1);

      while (true){
        digitalWrite(LED, HIGH);
        rf95.send(msg, strlen(msg));
        rf95.waitPacketSent();

        if (!rf95.waitAvailableTimeout(10000)){
          blinkLED();
          return;
        }
        uint8_t len  = sizeof(msg);
        if (!rf95.recv(msg, &len)){
          blinkLED();
          return;
        }

        break;
      }
    }else{
      timeOut += 1;
      delay(1);
      if (timeOut > 10000){
        sending = false; 
        timeOut = 0;
        Serial.println("Time Out");
      }
      return;
    }
  }
  
}

