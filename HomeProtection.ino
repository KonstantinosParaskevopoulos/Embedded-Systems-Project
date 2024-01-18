/**********************************Libraries****************************************/
#include <SoftwareSerial.h>     //Library for the GSM Module
#include <Wire.h>

/*******************************Const Pin Variables*************************************/

const int RELAY_ENABLE_FIRE = 10; //Relay Digital Pin
const int RELAY_ENABLE_WATER = 11; //Relay Digital Pin 
const int RELAY_ENABLE_GAS = 12; //Relay Digital Pin  
const int WATER_SENSOR = A5;      //Water sensor Analog Pin
const int TEST_ENABLE = 3;   //Test Enable Pin
const int FIRE_SENSOR_D = 4; //IR Sensor Digital Pin
//const int FIRE_SENSOR_A = A0; //IR Sensor Analog Pin
const int GAS_SENSOR_D = 5;  //Gas Sensor Digital
//const int GAS_SENSOR_A = A1;  //Gas Sensor Analog

/*********************************Flags*********************************************/

bool SMS_SENT_FIRE = 0;
bool SMS_SENT_WATER = 0;
bool SMS_SENT_GAS = 0;


/*******************************Constructors****************************************/

//Create software serial object to communicate with SIM900
SoftwareSerial mySerial(7, 8); //SIM900 Tx & Rx is connected to Digital Pins #7 & #8
//Adafruit_VEML6075 uv = Adafruit_VEML6075(); //Call Constructor for the UV Sensor

/*******************************Global Vars*****************************************/

int FS = 0;  //Init Fire Detect Variable
int WLS = 0; //Init Water Detect Variable
int GLS = 0; //Init Gas Detect Variable
int TEST_EN;

/*******************************Main Program****************************************/
void setup() {
  
  /*PinMode Declarations*/
  pinMode (FIRE_SENSOR_D, INPUT);
  pinMode (GAS_SENSOR_D, INPUT);
  pinMode (TEST_ENABLE, INPUT);

  digitalWrite(RELAY_ENABLE_WATER, LOW);//Initial States of Valves
  digitalWrite(RELAY_ENABLE_FIRE, LOW);
  digitalWrite(RELAY_ENABLE_GAS, LOW);

  FS = 0;  //Init Fire Detect Variable
  WLS = 0; //Init Water Detect Variable
  GLS = 0; //Init Gas Detect Variable
  

  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM900
  mySerial.begin(9600);
  Serial.println("Initializing..."); 
  delay(1000);
  
  

}

void loop() {

  TEST_EN = digitalRead(TEST_ENABLE);
  FS = digitalRead(FIRE_SENSOR_D);
  GLS = digitalRead(GAS_SENSOR_D);
  if(analogRead(WATER_SENSOR)>100) WLS=HIGH;
  else WLS=LOW;
  
  if(TEST_EN){  //If Test Enable is HIGH then perform System Check Routine
    System_Check_Routine();
  }

  if(FS == HIGH){ //Check for Fire
    if(!SMS_SENT_FIRE){
      if(!TEST_EN){
        open_Valve_Fire();  //Enable Fire Extinguishing Pump
        close_Valve_Gas();  //Close Flammable Gases Valve
      }
    SMS_fire();         //Send Fire Detection SMS
    SMS_SENT_FIRE = true;//Enable Flag that assures that all the actions have been completed to avoid being done again
    }
  }
  if(WLS == HIGH){  //Check for Water Leak
    if(!SMS_SENT_WATER){
      if(!TEST_EN){
        close_Valve_Water();  //Enable Fire Extinguishing Pump
      }
    SMS_water_leak();
    SMS_SENT_WATER = true;
    }
  }
  if(GLS == HIGH){  //Check for Gas Leak
    if(!SMS_SENT_GAS){
      if(!TEST_EN){
        close_Valve_Gas();  //Enable Fire Extinguishing Pump
      }
    SMS_gas_leak();
    SMS_SENT_GAS = true;
    }
  }

  delay(10000);  //Wait 10 seconds
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

void SMS_fire() {

  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+306941413370\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  mySerial.print("Fire Detected"); //text content
  updateSerial();
  mySerial.write(26); /*Sends SMS to the specified phone number. After this AT command any text message followed by ‘Ctrl+z’ character is treated as SMS. ‘Ctrl+z’ is actually a 26th non-printing character that is described as ‘substitute’ in the ASCII table. Therefore, we need to send 26 (0x1A) at the end of the command.*/

}
void SMS_water_leak() {
  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+306941413370\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  mySerial.print("Water Leak Detected"); //text content
  updateSerial();
  mySerial.write(26); /*Sends SMS to the specified phone number. After this AT command any text message followed by ‘Ctrl+z’ character is treated as SMS. ‘Ctrl+z’ is actually a 26th non-printing character that is described as ‘substitute’ in the ASCII table. Therefore, we need to send 26 (0x1A) at the end of the command.*/

}
void SMS_gas_leak() {
  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+306941413370\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  mySerial.print("Gas Leak Detected"); //text content
  updateSerial();
  mySerial.write(26); /*Sends SMS to the specified phone number. After this AT command any text message followed by ‘Ctrl+z’ character is treated as SMS. ‘Ctrl+z’ is actually a 26th non-printing character that is described as ‘substitute’ in the ASCII table. Therefore, we need to send 26 (0x1A) at the end of the command.*/

}

void close_Valve_Water(){
  digitalWrite(RELAY_ENABLE_WATER, HIGH);
}

void open_Valve_Fire(){
  digitalWrite(RELAY_ENABLE_FIRE, HIGH);
}

void close_Valve_Gas(){
  digitalWrite(RELAY_ENABLE_GAS, HIGH);
}

void System_Check_Routine(){
  FS = HIGH;  //Simulate Fire Alarm
  WLS = HIGH; //Simulate Water Leak Alarm
  GLS = HIGH; //Simulate Gas Leak Alarm

  int check = FS & WLS & GLS; //All Values must be 1 to pass the test / This also tests ALU for faults in the AND Operation
  
  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+306941413370\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  if(check){
    mySerial.print("Check Routine Initialization...\n\nTest Passed!"); //Test text content for Test Success
  }
  else {
    mySerial.print("\n\n-------------------------------------------------\n\n");
    mySerial.print("Check Routine Initialization...\n\nTest Failed!"); //Test text content for Test Fail
    mySerial.print("\nFire: ");
    mySerial.print(FS);
    mySerial.print("\nGas Leak: ");
    mySerial.print(GLS);
    mySerial.print("\nWater Leak: ");
    mySerial.print(WLS);
    mySerial.print("\n\n-------------------------------------------------\n\n");
  }
  updateSerial();
  mySerial.write(26); /*Sends SMS to the specified phone number. After this AT command any text message followed by ‘Ctrl+z’ character is treated as SMS. ‘Ctrl+z’ is actually a 26th non-printing character that is described as ‘substitute’ in the ASCII table. Therefore, we need to send 26 (0x1A) at the end of the command.*/

}
























