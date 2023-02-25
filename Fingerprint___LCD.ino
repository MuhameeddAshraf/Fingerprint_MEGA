//https://github.com/MuhameeddAshraf//LCD_Fingerprint

#include <DFRobot_ID809.h>
#include <Servo.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>

Servo myservo;
int pos = 0;


// Create LCD object : Use 0x27 If 0x3F Doesn't work
LiquidCrystal_I2C lcd(0x27, 20, 4);

#if ((defined ARDUINO_AVR_UNO) || (defined ARDUINO_AVR_NANO))
    #include <SoftwareSerial.h>
    SoftwareSerial Serial1(2, 3);  //RX, TX
    #define FPSerial Serial1
#else
    #define FPSerial Serial1
#endif

DFRobot_ID809 fingerprint; //String desc;

void setup(){
  myservo.attach(9); /*Servo Control Pin*/ 
  lcd.init();
  lcd.backlight();
  printWelcomeMessage();

  /*Init print serial port*/
  Serial.begin(9600);
  /*Init FPSerial*/
  FPSerial.begin(115200);
  /*Take FPSerial as communication serial port of the fingerprint module*/
  fingerprint.begin(FPSerial);
  
  while(!Serial);  /*Wait for Serial to open*/
  /*Test whether the device can properly communicate with mainboard
   Return true or false*/
  while(fingerprint.isConnected() == false){
    Serial.println("Communication with device failed, please check connection");
    /*Get error code information*/
    //desc = fingerprint.getErrorDescription();
    //Serial.println(desc);
    delay(1000);
  }
}

void loop(){
  uint8_t ret = 0;

  fingerprint.ctrlLED(/*LEDMode = */fingerprint.eBreathing, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
  Serial.println("Please press down your finger");
  /*Capture fingerprint image, Disable the collection timeout function 
    If succeed return 0, otherwise return ERR_ID809
   */
  if((fingerprint.collectionFingerprint(/*timeout=*/0)) != ERR_ID809){
    /*Set fingerprint LED ring to quick blink in yellow 3 times*/
    fingerprint.ctrlLED(/*LEDMode = */fingerprint.eFastBlink, /*LEDColor = */fingerprint.eLEDYellow, /*blinkCount = */3);
    Serial.println("Capturing succeeds");
      Serial.println("Please release your finger");
    /*Wait for finger to release
      Return 1 when finger is detected, otherwise return 0 
     */
    while(fingerprint.detectFinger());

    /*Compare the captured fingerprint with all the fingerprints in the fingerprint library 
      Return fingerprint ID(1-80) if succeed, return 0 when failed 
     */
    ret = fingerprint.search();
    /*Compare the captured fingerprint with a fingerprint of specific ID 
      Return fingerprint ID(1-80) if succeed, return 0 when failed 
     */
    //ret = fingerprint.verify(/*Fingerprint ID = */1);  
    if(ret != 0){
      /*Set fingerprint LED ring to always ON in green */
      fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDGreen, /*blinkCount = */0);
      Serial.print("Matching succeeds,ID=");
      Serial.println(ret);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   WELCOME TO SSA ");
      lcd.setCursor(0, 1);
      lcd.print("   Access Granted!");
      lcd.setCursor(0, 2);
      lcd.print(" The Arm is opening! ");
      lcd.setCursor(0, 3);
      lcd.print("<><><><><><><><><><>");

      
      for (pos = 0; pos <= 90; pos += 1) {
        myservo.write(pos);              
        delay(5);                       
        }
       delay(5000);
       for (pos = 90; pos >= 0; pos -= 1) {
       myservo.write(pos);              
       delay(5);                       
      }
      
     delay(5000);  //To fix (LCD, Solenoid) issue
     printWelcomeMessage(); 
    }
    else{
      /*Set fingerprint LED ring to always ON in red*/
      fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDRed, /*blinkCount = */0);
      Serial.println("Matching fails");
    }
  }else{
    Serial.println("Capturing fails");
    /*Get error code information*/
    //desc = fingerprint.getErrorDescription();
    //Serial.println(desc);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   WELCOME TO SSA ");
    lcd.setCursor(0, 1);
    lcd.print("      <SORRY>");
    lcd.setCursor(0, 2);
    lcd.print("   Access Denied!");
    lcd.setCursor(0, 3);
    lcd.print(" </> Scan Agian! </>");
    delay(5000);
    printWelcomeMessage();

 
  }
  Serial.println("-----------------------------");
  delay(1000);
}

void printWelcomeMessage() {
  lcd.clear();
  lcd.print("   WELCOME TO SSA ");
  lcd.setCursor(0, 1);
  lcd.print("  <Access Control>");
  lcd.setCursor(0, 2);
  lcd.print("Scan Your Fingerprint!");
  lcd.setCursor(0, 3);
  lcd.print("<><><><><><><><><><>");
}
