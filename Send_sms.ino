#include <SoftwareSerial.h>

SoftwareSerial smsSerial(7, 8); // RX and TX pins to communicate with SIM800L module

String number = "525785401"; // Replace this 10-digits with  recipient's number


void setup()

{
  Serial.begin(9600);
  smsSerial.begin(9600);
  
}

void loop()
{
  //Sends an sms
  
      smsSerial.println("AT+CMGF=1"); // Set the Mode as Text Mode
      delay(150);
      smsSerial.println("AT+CMGS=\"+972" + number + "\""); // Specify the recipient's number in international format by replacing the 91
      delay(150);
      smsSerial.print("hey"); // Enter the custom message
      delay(150);
      smsSerial.write((byte)0x1A); // End of message character 0x1A : Equivalent to Ctrl+z
      delay(50);
      smsSerial.println();
      delay(3000);
    
}
