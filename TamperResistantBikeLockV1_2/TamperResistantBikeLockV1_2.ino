/*-------------------------------
Tamper Resistant Bike Lock
Sam McDowell
Updated 12/18/22
-------------------------------*/

// necessary inclusions
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

// pin declaration
int buttonPin = 6;  
int buzzerPin = 3;
int servoPin = 5;
int circuitBreakPin = 4;
#define SS_PIN 10
#define RST_PIN 9

// constant declaration
int angleClosed = 120;
int angleOpen = 150;
int rotateSpeed = 15;
bool open = true;
bool change = false;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // intialize RFID sensor
Servo servo; // initialize servo

void setup() {

  // set pinmodes
  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT); 
  pinMode(circuitBreakPin, INPUT);

  servo.attach(servoPin); // set up servo
  openLock(); // start in open position

  Serial.begin(9600);

  // set up RFID
  SPI.begin();      
  mfrc522.PCD_Init(); 
}

void loop(){

  // BUZZER OPERATION
  if (digitalRead(circuitBreakPin) == LOW) {
    tone(buzzerPin, 3000, 500);
    if (open == false) {
      tone(buzzerPin, 3000, 500);
    }
  }

  // BUTTON OPERATION
  if (digitalRead(buttonPin) == HIGH) { 
    if (open == true) {
      open = false;
      closeLock();
    }
  }

  // RFID OPERATION
  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;}
  if ( ! mfrc522.PICC_ReadCardSerial()) {return;}

  // read RFID
  String content= "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  // check if code matches
  if (content.substring(1) == "D3 C6 0F 1E")
  {
    // open the lock if closed
    if (!open) {
      open = true;
      openLock();
    }
  }
  // buzz on incorrect code
  else {
      tone(buzzerPin, 1000, 500);
  }
}

// move servo from open to closed
void closeLock() {
  for(int angle = angleClosed; angle < angleOpen; angle++)    
    {          
      servo.write(angle);  
      delay(rotateSpeed);       
    } 
}

// move servo from closed to open
void openLock() {
  for(int angle = angleOpen; angle > angleClosed; angle--)  
    {               
      servo.write(angle);    
      delay(rotateSpeed);                   
    } 
}
