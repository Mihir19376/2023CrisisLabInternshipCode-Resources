
// Programme ONE - ECHO
#include <RH_RF95.h>
#define RF95_CS 10
#define RF95_INT 2
#define RF95_RST 9
#define RF95_FREQ 915.0

RH_RF95 rf95(RF95_CS, RF95_INT);
uint8_t data[RH_RF95_MAX_MESSAGE_LEN];

void setup()
{
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
}

void loop(){
  if (!rf95.available()){
    return;
  }
  uint8_t len  = sizeof(data);
  if (!rf95.recv(data, &len)){
    return;
  }

  // Serial.print("got request: ");
  Serial.println((char*)data);
  rf95.send(data, len);
  rf95.waitPacketSent();
}
