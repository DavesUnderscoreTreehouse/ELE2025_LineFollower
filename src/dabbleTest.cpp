/*#include <Dabble.h>

bool   response = 0;
String response_data = "";
String send_data="";

void checkATresponse();
void readBaudRate();
void writeBaudRate();

void setup() {
  Serial.begin(250000);
  Serial3.begin(38400);
  checkATresponse();
  // put your setup code here, to run once:
}

void loop() {
  Serial.println("Enter 1 to read baudrate and 2 to write baudrate");
  while(Serial.available()==0);
  while(Serial.available()!=0)
  {
    uint8_t a=Serial.read();
    if(a == '1')
    {
      readBaudRate();
    }
    if(a == '2')
    {
      writeBaudRate(); 
    }
  }
}

void checkATresponse()
{
  long lastTime=0;
  while (!(response))
  {
    Serial3.print("AT\r\n");
    response_data = Serial3.readString();
    if (response_data != '\0')                
    {
    Serial.print("Command sent: "); 
    Serial.println("AT");
    Serial.print("Command received: ");
    Serial.println(response_data);
    response++;
    }
  }
  response=0;
}

void readBaudRate()
{
  while (!(response))
  {
    Serial3.print("AT+UART?\r\n");
    response_data = Serial3.readString();
    if (response_data != '\0')                
    {
    Serial.print("Command sent: ");   
    Serial.println("AT+UART?");
    Serial.print("Command received: ");
    Serial.println(response_data);
    response++;
    }
  }
  response=0;
}

void writeBaudRate()
{
  Serial.println("Enter any baudrate as per requirement. Some of the most commonly used baudrates are 9600, 38400,57600 and 115200.");
  Serial.println("Waiting for you to enter Baudrate...");
  while(Serial.available()==0);
  while(Serial.available()!=0)
  {
    send_data=Serial.readString();
    Serial.println(send_data);
  }
   while (!(response))
  {
    Serial3.print(String("AT+UART="+send_data+",0,0\r\n"));
    response_data = Serial3.readString();
    if (response_data != '\0')                
    {
    Serial.print("Command sent: ");   
    Serial.println(String("AT+UART="+send_data+",0,0"));
    Serial.print("Command received: ");
    Serial.print(response_data);
    response++;
    }
    if(response_data == "OK\r\n")
    {
      Serial.println("Baudrate changed successfully");
    }
    else
    {
      Serial.println("Error");
    }
  }
  response=0;
}*/

/*
   Led Brightness Control Module allows user to control any digital pin on their board. They can turn pin ON or OFF, can
   also vary its PWM if that functionality is supported on that pin.

   You can reduce the size of library compiled by enabling only those modules that you want to
   use.For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

   Explore more on: https://thestempedia.com/docs/dabble/led-brightness-control-module/
*/
#define CUSTOM_SETTINGS
#define INCLUDE_LEDCONTROL_MODULE
#include <Dabble.h>

void setup() {
  Serial.begin(250000);     // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin(9600);     //Enter baudrate of your bluetooth. Connect bluetooth on Bluetooth port present on evive.

}

void loop() {
  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  Serial.print("Led:");
  Serial.print(LedControl.getpinNumber());
  Serial.print('\t');
  Serial.print("State:");          //0 if led is Off. 1 if led is On.
  Serial.print(LedControl.getpinState());
  Serial.print('\t');
  Serial.print("Brightness:");
  Serial.println(LedControl.readBrightness());
}