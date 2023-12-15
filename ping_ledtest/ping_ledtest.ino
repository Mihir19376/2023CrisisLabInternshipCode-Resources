// Programme TWO - PING - Client
// This arduino file must be uploaded to the client arduino's that are connected to the Pi/Shakes.
// This file will siphon the data from the Pi's/Shake's to the engineer's server arduino using the LoRa communication technology. 

#include <RH_RF95.h> // Including the Radio Head library for LoRa communication
#define RF95_CS 10 // Chip Select pin for RF95 LoRa module
#define RF95_INT 2 // Interrupt pin for RF95 LoRa module
#define RF95_RST 9 // Reset pin for RF95 LoRa module
#define RF95_FREQ 915.0 // Frequency for RF95 LoRa module

#define LED 4 // This is the LED indicator

RH_RF95 rf95(RF95_CS, RF95_INT); // Initializing RF95 LoRa module
uint8_t data[RH_RF95_MAX_MESSAGE_LEN]; // Buffer to store received data

bool sending = false; // Boolean to indicate whether the data is currently being transmitted
String clientId = "Client #1"; // The client ID unique to this client. Each Client much have a unique ID
int timeOut = 0; // Timeout counter

void setup() {
  timeOut = 0;
  sending = false;
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
}

/// @brief This just blinks the indicator LED. It also runs the delays needed when communication doesn't work
void blinkLED() {
  digitalWrite(LED, LOW);
  delay(500);
  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, HIGH);
    delay(100);
  }
  delay(500);
}

void loop() {
  if (!sending) { // If this is currently not transmitting data, listen for it
    // If not available, re-run loop
    if (!rf95.available()) {
      return;
    }
    uint8_t len = sizeof(data);
    if (!rf95.recv(data, &len)) {
      return;
    }

    String receivedMessage = (char*)data; // Converting received data to a String

    receivedMessage.trim(); // Removing leading/trailing whitespaces
    Serial.println(receivedMessage);
    Serial.println(receivedMessage == clientId); // Checking if the received message matches the client ID

    if (receivedMessage == clientId) {
      sending = true; // Establishing connection with the server
      Serial.println("Connection Established"); // This message will signal the Pi/Shake to begin offloading it's data. 
    }
  } 
  else { // The connection is now established, the data received from the Pi/Shake will now be transmitted to the server
    if (Serial.available() > 0) {
      timeOut = 0; // Reset the counter to signal that the Pi/Shake data that is printed to the serial monitor is being received. 
      delay(100); // Wait for the entire line to be printed 
      String data = Serial.readString(); // Store the line
      Serial.println(data);

      char msg[300];
      data.toCharArray(msg, data.length() + 1); // Convert it into an array of characters

      // Send the data and break once it has been sent
      while (true) {
        digitalWrite(LED, HIGH);
        rf95.send(msg, strlen(msg));
        rf95.waitPacketSent();

        if (!rf95.waitAvailableTimeout(10000)) { // Maximum wait time until the data has been sent, after which, it will try again
          blinkLED();
          return;
        }
        uint8_t len = sizeof(msg);
        if (!rf95.recv(msg, &len)) {
          blinkLED();
          return;
        }

        break;
      }
    } 
    else { // If there was no data from the Pi/Shake printed to the serial port, then increase the counter by 1 every millisecond this occurs
      timeOut += 1;
      delay(1);
      // If the time is above ten seconds, then time out the system and set sending to false (all the data has been sent, and the client has disconnected) 
      if (timeOut > 10000) {
        sending = false;
        timeOut = 0;
        Serial.println("Time Out");
      }
      return;
    }
  }
}
