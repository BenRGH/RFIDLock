/*
 * This code makes use of the RC522 RFID module and a servo, when one of the 
 * approved access cards are read by the RFID module it rotates the servo a
 * a number of degrees.
 * 
 * Portions are inspired by:
 * https://github.com/miguelbalboa/rfid/
 * 
 * BenRGH 2018
 * MIT license, free to distribute and modify.
 * 
 */

#include <SPI.h>
#include <MFRC522.h> // Included in this repo as rfid.zip if you didn't have it
#include <Servo.h>

// Define pins for RFID module
constexpr uint8_t RST_PIN = 5;
constexpr uint8_t SS_PIN = 53;

// Servo vars
boolean doorOpen = false; // Door is open
int servoLocked = 10; // Degree for when the servo is in the locked pos
int servoOpen = 120; // Degree for when the servo is in the open pos
int closeLidDelay = 10; // Delay before locking, gives time to close

// Allowed UID codes - serial shows codes for cards if more needed
unsigned long allowedUID[5] = {4294945321,4294948233,4294964812,20495,19390};

// Instancing
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
Servo lServo; // Lock servo

void setup(){
  Serial.begin(9600);
  SPI.begin(); // Init SPI
  mfrc522.PCD_Init(); // Initialise the RFID module
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); //increases reading distance
  lServo.attach(9); // Set servo pin
  lServo.write(servoLocked); // Set init lock position of the servo motor
  
}

unsigned long getID(){
  // Modified from here:
  // https://stackoverflow.com/questions/32839396/how-to-get-the-uid-of-rfid-in-arduino
 
  if ( ! mfrc522.PICC_ReadCardSerial()) { // Reads serial of RFID tag
    return -1;
  }

  // The following magic converts the UID of the card into a long
  unsigned long hex_num;
  hex_num =  mfrc522.uid.uidByte[0] << 24; // Ignore this error
  hex_num += mfrc522.uid.uidByte[1] << 16;
  hex_num += mfrc522.uid.uidByte[2] <<  8;
  hex_num += mfrc522.uid.uidByte[3];
  mfrc522.PICC_HaltA(); // Stop reading
  return hex_num;
}

void openClose(){
  //If the door is open then calling this locks it and vice versa
  
  if (doorOpen){
    delay(closeLidDelay); // Gives time to close lid
    lServo.write(servoLocked); // Move servo to locked pos
    delay(15); // let servo move
    doorOpen = false;
  }else{
    lServo.write(servoOpen); // Move servo to unlocked pos
    delay(15); // let servo move
    doorOpen = true;
  }
}

void loop(){
  // Wait for card
  if (mfrc522.PICC_IsNewCardPresent()) {
    // Card detected
    unsigned long uid = getID();
    if (uid != -1){
      // If the card is normal then it prints the UID to serial...
      Serial.print("card id:");
      Serial.println(uid);

      // And then checks if it's an authorised card
      for (int i = 0; i < 4; i++){
        if (uid == allowedUID[i]){
          // Unlock/Lock if the card is authorised
          openClose();
          return;
        }
        // Do nothing if it isn't
      }
    }
  }

  
  
}


// lServo.write(ANGLE BETWEEN 0 AND 180)
// should probs use delay for it
