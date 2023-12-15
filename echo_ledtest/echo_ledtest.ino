// Programme ONE - ECHO - Server
// This arduino file will be uploaded to the server arduino with the engineer
// This file will establish a connection with a client and will receive and print all the data-
// -to the serial monitor, where the ArduinoToFiles python program will store the data in files

#include <RH_RF95.h> // Including the Radio Head library for LoRa communication
#define RF95_CS 10 // Chip Select pin for RF95 LoRa module
#define RF95_INT 2 // Interrupt pin for RF95 LoRa module
#define RF95_RST 9 // Reset pin for RF95 LoRa module
#define RF95_FREQ 915.0 // Frequency for RF95 LoRa module

RH_RF95 rf95(RF95_CS, RF95_INT); // Initializing RF95 LoRa module
uint8_t data[RH_RF95_MAX_MESSAGE_LEN]; // Buffer to store received data

bool requesting = true; // Boolean to indicate whether the data is currently being transmitted
int timeOut = 0; // Timeout counter

void setup()
{
  timeOut = 0;
  requesting = true;
  Serial.begin(9600);
  // Initializing the LoRa module
  pinMode(RF95_RST, OUTPUT);
  digitalWrite(RF95_RST, HIGH);
  delay(10);
  digitalWrite(RF95_RST, LOW);
  delay(10);
  digitalWrite(RF95_RST, HIGH);
  delay(10);
  rf95.init();
  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23); // Setting LoRa transmission power to 23 dBm (max power and range)
  rf95.setModemConfig(0xc0);
}

void loop(){
  // If we are no longer requesting (sending out a connection message) then:
  if (!requesting){
    // Increase the timer by 1 if we still haven't received anything
    if (!rf95.available()){
      timeOut += 1;
      delay(1);
      // If the time is above ten seconds, then time out the system and set requesting to true
      // (all the data has been sent, and the client has disconnected). We can now request for another client once more
      if (timeOut > 10000){
        requesting = true; 
        timeOut = 0;
        Serial.println("Time Out");
      }
      return;
    }
    timeOut=0; // If we reach this point, it means we did receive something, and we reset the countdown timer
    uint8_t len  = sizeof(data);
    if (!rf95.recv(data, &len)){  
      return;
    }

    Serial.println((char*)data);
    rf95.send(data, len);
    rf95.waitPacketSent();

  }
  else{ // We are requesting (entering wanted client ID through the serial monitor)
    if(Serial.available() > 0){
      delay(100);
      String data = Serial.readString(); // Store the clientID we have entered
      Serial.println(data);
      char msg[300];
      data.toCharArray(msg, data.length() + 1); // Convert that message into an array of characters

      // Send the data and break once it has been sent
      while (true){
        rf95.send(msg, strlen(msg));
        rf95.waitPacketSent();

        break;
      }
      requesting = false;
    }
  }
  
}
