/* Linksprite 

Infrared Camera Test Code for MEGA.  This is meant to be a functional test of your camera and SD card only... To be used to verify your
circuit and board before you use oap_motioncamera_mega (which is the full Ethernet Shield, Motion Detector, and camera)
As such, you don't really need any of the motion detector connections outlined below


  Notes: 
    Not all pins on the Mega and Mega 2560 support change interrupts, so only the following
      can be used for RX: 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

*/

#include <SoftwareSerial.h>
#include <SD.h>

int sdCardControlPin = 4;  // SD Card Control Pin

byte incomingbyte;
byte responseMessage[110]; // Used to hold responses

// Try using MEGA serial2 which should be: 17 (RX) and 16 (TX)

const unsigned long deadManTimer = 6000;
unsigned long startWaitTime;
int i=0; // loop

boolean SendResetCmd();

void setup()
{ 
  Serial.begin(9600);
  Serial2.begin(38400);
  
  // Set up SD Card
  pinMode(sdCardControlPin, OUTPUT);

  if (!SD.begin(sdCardControlPin)) {
    Serial.println("  Card failed, or not present"); // don't do anything more:
    return;
  }
  Serial.println("SD Card initialized.");
 
  // WARNING!  All of this camera code MUST be AFTER the SD Card initialization!!
  delay(5000);  // Camera needs AT LEAST five seconds to shut down!
    
 
  // Wait for proper response from camera
  Serial.println("Waiting for camera init end"); 
  startWaitTime = millis();
  i = 0;
  while(i <= 100 && (millis()-startWaitTime < deadManTimer))
  {
    if (Serial2.available()>0) {
      incomingbyte=Serial2.read();
      responseMessage[i] = incomingbyte;
      //Serial.print(responseMessage[i], HEX);
      Serial.write(responseMessage[i]);
      if (incomingbyte == 0x0A)
        Serial.println();
      else {
        //Serial.print(" ");
      }
      i++;
    }
  }
  
  // We are most often getting this:
  // 0 26 0 0 56 43 30 37 30 33 20 31 2E 30 30 76 
  // 0 & 0  0 V  C  0  7  0  3     1  .  0  0  L
  // Sometimes this:
  // 0 56 43 30 37 30 33 20 31 2E 30 30 D A 4E 6F
  // 0  V  C  0  7  0  3    1  .   0  0 C L N  o
  // With NO RESET, we get this (changes sometimes):
  // 72 6C 20 69 6E 66 72 D A 4D 49 33 36 30 D A 36 32 35 D A 49 0 56 43 30 37 30 33 20 31 2E 30 30 D A 4E 6F 20 63 74
  // r  m    i   n  f  r      M  I  3  6  0      6  2  5      I    V  C  0  7  0  3      1  .  0  0      N  o    c  t
//0 56 43 30 37 30 33 20 31 2E 30 30 D
//A 4E 6F 20 63 74 72 6C 20 69 6E 66 72 D
//A 4D 49 33 36 30 D
//A 36 32 35 D
//A 49 6E 69 74 20 65 6E 64 D
//A
// Which is:
//V C 0 7 0 3   1 . 0 0 
//N o   c t r l   i n f r 
//M I 3 6 0 
//6 2 5 
//I n i t   e n d 
 

  
  Serial.println("");
  // Did we get the expected response?
  if (responseMessage[i-2] == 0x0D && responseMessage[i-1] == 0x0A) {
    Serial.println("Camera is initialized");
  }
  else
    Serial.println("WARNING: Camera may have initialized early");  

 
      
}



void loop() 
{
  Serial.println("Beginning Loop");
 
   //dumpCameraInput();
   Serial.println("Resetting Camera");
   SendResetCmd();

//   Serial.println("Waiting for camera reset response"); 
//   // Regardless of what the manual says, this is what you routinely get back from a camera reset
//    //v &  VC0703 1.00
//    //No ctrl infr
//    //MI360
//    //625
//    //Init end
//   // We'll look for the v (0x76) and the & (0x26)
//   
//  startWaitTime = millis();
//  i = 0;
//  while(i <= 100 && (millis()-startWaitTime < deadManTimer))
//  {
//    if (Serial2.available()>0) {
//      incomingbyte=Serial2.read();
//      responseMessage[i] = incomingbyte;
//      //Serial.print(responseMessage[i], HEX);
//      Serial.write(responseMessage[i]);
//      if (incomingbyte == 0x0A)
//        Serial.println();
//      else {
//        //Serial.print(" ");
//      }
//      i++;
//    }
//  }
// 
//   Serial.println("");
//  // Did we get the expected response?
//  if (responseMessage[0] == 0x76 && responseMessage[2] == 0x26) {
//    Serial.println("Camera has been reset");
//  }
//  else
//    Serial.println("WARNING: Unexpected response to camera reset");  
 
     Serial.println("Sleeping before another try...");
     delay(20000); 
     
}



//Send Reset command
boolean SendResetCmd()
{
     // Dumps the camera input buffer, issues a reset command, and looks for the response
     // Returns true if we received the expected response
     // As of Arduino 1.0 the 'BYTE' keyword is no longer supported.
     // Please use Serial.write() instead.
     // we must cast to (byte) to tell the compiler how to interpret these ZERO values
     // can use (byte) but it is recommended to use (uint8_t)
     // We Send: 65 00 26 00
     // It Returns: 76 00 26 00
     //              v    &

      // First, flush the buffer
      dumpCameraInput();

      Serial2.write(0x56);
      Serial2.write((uint8_t)0x00);
      Serial2.write(0x26);
      Serial2.write((uint8_t)0x00);

   startWaitTime = millis();
    i = 0;
    while(i <= 4 && (millis()-startWaitTime < deadManTimer))
    {
      if (Serial2.available() > 0) {
        incomingbyte=Serial2.read();
        responseMessage[i] = incomingbyte;
        //Serial.print(responseMessage[i], HEX);
        Serial.write(responseMessage[i]);
        if (incomingbyte == 0x0A)
          Serial.println();
        else {
          //Serial.print(" ");
        }
        i++;
      }
    }
    
    if (responseMessage[0] == 0x76 && responseMessage[2] == 0x26) {
    Serial.println("Camera has been reset");
    return true;
    }
    else
    {
      Serial.println("Warning: Expected reset response not found");
      return false;
    }           
}

void dumpCameraInput()
{
  // Use this before issuing a command to be sure the buffer is empty.
  Serial.print("Dumping...");
  while(Serial2.available() > 0) 
     {
        incomingbyte=Serial2.read();
        Serial.write(incomingbyte);
      }  
  Serial.println("... END DUMPING");
  
}
