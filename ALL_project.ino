#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <LowPower.h>
#include <Battery.h>
#include <Wire.h>

char phone_no[] = "972525785401"; //replace with phone no. to get sms red1 white0

TinyGPS gps;  //Creates a new instance of the TinyGPS object

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
char tmp_str[7]; // temporary variable used in convert function
char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

SoftwareSerial SIM900(7, 8);
Battery battery(3400, 4200, A0);

//  variables
String incomingData, ondata,Gdata;
String Battery;
String Message;
String GPS_MSG;
String number = "525785401"; // Replace this 10-digits with  recipient's number
int flag = 0;
int sflag=0;
int Acc1, Acc2 = 0;
int x1, x2, y1, y2, z1, z2;




void setup() {
  SIM900.begin(9600);
  Serial.begin(9600);
  delay(2000);
  while (!Serial);
  battery.begin(5000, 1.0);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);


}



void loop() {
  Serial.println("Entering Sleep mode");
  delay(100);
  for (int i = 0; i < 1; i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
  delay(100);
  incomingData = Check_sms();
  delay(2000);
  Serial.println("Checking SMS");
  delay(1000);

  if (incomingData.indexOf("ON") >= 0 && flag == 0) {
    ON();
  }
  else if (incomingData.indexOf("OFF") >= 0 && flag == 1) {
    OFF();
  }
  else
    Serial.println("STANDBY MODE");
  delay(1000);
}


String Check_sms() {
  if (SIM900.available() > 0)
  {
    incomingData = SIM900.readString(); // Get the data from the serial port.
    delay(10);
    Gdata=SIM900.readString();
    delay(10);
  }
  return incomingData;
}

void Send_sms(String Message) {

  SIM900.println("AT+CMGF=1"); // Set the Mode as Text Mode
  delay(150);
  SIM900.println("AT+CMGS=\"+972" + number + "\""); // Specify the recipient's number in international format by replacing the 91
  delay(150);
  SIM900.print(Message); // Enter the custom message
  delay(150);
  SIM900.write((byte)0x1A); // End of message character 0x1A : Equivalent to Ctrl+z
  delay(50);
  SIM900.println();
  delay(3000);
}

int Acclerometer() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7 * 2, true); // request a total of 7*2=14 registers

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accelerometer_x = Wire.read() << 8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read() << 8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read() << 8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)

  // print out data
  Serial.print("aX = "); Serial.print(convert_int16_to_str(accelerometer_x));
  Serial.print(" | aY = "); Serial.print(convert_int16_to_str(accelerometer_y));
  Serial.print(" | aZ = "); Serial.print(convert_int16_to_str(accelerometer_z));
  Serial.println();
  x1 = accelerometer_x;
  y1 = accelerometer_y;
  z1 = accelerometer_z;
  return (sqrt(pow(accelerometer_x, 2) + pow(accelerometer_y, 2) + pow(accelerometer_z, 2)));
  // delay
  delay(1000);
}

String Check_battery() {
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
  return battery.level();
}

void ON_MODE() {
  while (!(ondata.indexOf("OFF") >= 0)) {
    delay(100);
    ondata = Check_sms();
    delay(2000);
    Serial.println("Entering Sleep mode");
    delay(100);
    for (int i = 0; i < 1; i++) {
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
    Acc1 = Acclerometer();
    if ((Acc2 - Acc1 > 2000) || (x2 - x1 > 2000) || (y2 - y1 > 2000) || (z2 - z1 > 2000)) {
      while (1) {
        delay(100);
        Gdata = Check_sms();
        delay(2000);
        if (Gdata.indexOf("OFFGPS") >= 0)
        break;
        for (int i = 0; i < 1; i++) {
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
        }
      Serial.println("THE BIKE ARE BEING STOLEN ! ! ! ");
      Message = ("THE BIKE ARE BEING STOLEN ! ! !");
      Send_sms(Message);
      GPS();
      delay(4000);
      }
    }
    else {
      Acc2 = Acc1;
      x2 = x1;
      y2 = y1;
      z2 = z1;
    }
    Serial.println(Acc1);
    Serial.println(Acc2);
    Serial.println(x1);
    Serial.println(x2);
    Serial.println(y1);
    Serial.println(y2);
    Serial.println(z1);
    Serial.println(z2);
    delay(1000);
  }
  OFF();
}
void ON() {
  Serial.println("flag=1");
  flag = 1;
  Message = "ON MODE";
  Serial.println("ON MODE");
  delay(1000);
  Send_sms(Message);
  ON_MODE();
}
void OFF() {
  Serial.println("flag=0");
  flag = 0;
  Message = "OFF MODE";
  Serial.println("OFF MODE");
  delay(2000);
  Send_sms(Message);
  Acc2 = 0;
  Acc1 = 0;
  x1=0;
  x2=0;
  y1=0;
  y2=0;
  z1=0;
  z2=0;
  ondata = "Zero";
  Gdata="Zero";
  incomingData = "OFF";
}

void GPS() {
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      Serial.print(c);
      if (gps.encode(c))
        newData = true;
    }
  }

  if (newData)      //If newData is true
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    SIM900.print("AT+CMGF=1\r");
    delay(400);
    SIM900.print("AT+CMGS=\"");
    SIM900.print(phone_no);
    SIM900.println("\"");

    delay(300);
    SIM900.print("http://maps.google.com/maps?q=loc:");

    // SIM900.print("Latitude = ");
    SIM900.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    //SIM900.print(" Longitude = ");
    SIM900.print(",");
    SIM900.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    delay(200);
    SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
    delay(200);
    SIM900.println();
    delay(100);

  }

  Serial.println(failed);
  // if (chars == 0)
  // Serial.println("** No characters received from GPS: check wiring **");
}
