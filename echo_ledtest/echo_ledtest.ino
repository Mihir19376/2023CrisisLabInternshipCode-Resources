
// Programme ONE - ECHO
#include <RH_RF95.h>
#define RF95_CS 10
#define RF95_INT 2
#define RF95_RST 9
#define RF95_FREQ 915.0

RH_RF95 rf95(RF95_CS, RF95_INT);
uint8_t data[RH_RF95_MAX_MESSAGE_LEN];

bool requesting = true;
int timeOut = 0;

void setup()
{
  timeOut = 0;
  requesting = true;
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
  rf95.setModemConfig(0xc0);
}

void loop(){
  // I need to request data from a specific client before running this code. 
  if (!requesting){
    if (!rf95.available()){
      timeOut += 1;
      delay(1);
      if (timeOut > 10000){
        requesting = true; 
        timeOut = 0;
        Serial.println("Time Out");
      }
      return;
    }
    timeOut=0;
    uint8_t len  = sizeof(data);
    if (!rf95.recv(data, &len)){  
      return;
    }

    // Serial.print("got request: ");
    Serial.println((char*)data);
    rf95.send(data, len);
    rf95.waitPacketSent();

  }
  else{
    if(Serial.available() > 0){
      delay(100);
      String data = Serial.readString();
      Serial.println(data);
      char msg[300];
      data.toCharArray(msg, data.length() + 1);

      while (true){
        rf95.send(msg, strlen(msg));
        rf95.waitPacketSent();

        break;
      }
      requesting = false;
    }
  }
  
}
