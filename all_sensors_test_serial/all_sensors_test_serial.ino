 /**************************************************************************************



**************************************************************************************/

#include "ClosedCube_MAX30205.h"

ClosedCube_MAX30205 max30205_bridge;
ClosedCube_MAX30205 max30205_arch;
ClosedCube_MAX30205 max30205_heel;



#include "Adafruit_SHTC3.h"

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

//Setup internal file system
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
using namespace Adafruit_LittleFS_Namespace;

#define FILENAME    "/config.txt"
File file(InternalFS);

//Reset current time to show reset, would be replaced by 
int current_time = 0;

//Set delay amount in seconds for initial measurements
int initial_delay = 10;
int delay_step = 5;
int max_delay = 10;
int delay_seconds = initial_delay;

//Prints data to serial OR sends data over bluetooth if flag == 1
int serial_flag = 1;

int password = 48;
int entered_password;
int password_buffer;

int temp_threshold = 1;
int humidity_threshold = 2;
float heeltemp, archtemp, bridgetemp;
float prev_heeltemp, prev_archtemp, prev_bridgetemp, prev_toetemp, prev_humi;

sensors_event_t humidity, temp; //Initialize humidt and temp for the shtc3 sensor


void setup()
{
  //Begin serial communications and print headers for data (space used as delimiter)
	Serial.begin(115200);
  //while (!Serial) delay(1000);
  //Serial.println("time humidity toetemp archtemp bridgetemp heeltemp");

  //Begin all of the temperature sensors
	max30205_bridge.begin(0x48);
  max30205_arch.begin(0x4C);
  max30205_heel.begin(0x4F);

  shtc3.begin();

  // Begin internal file system
  InternalFS.begin();

  prev_heeltemp = 0;
  prev_archtemp = 0;
  prev_bridgetemp = 0;
}

void loop()
{
  shtc3.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data

  archtemp = max30205_arch.readTemperature();
  bridgetemp = max30205_bridge.readTemperature();
  heeltemp = max30205_heel.readTemperature();

  if (serial_flag){
    //If flag is set, print all of the data out to serial

    //Pad current time with zeroes since matlab reads by number of characters (need to always print same number of characters)
    if(current_time < 100000) {Serial.print("0");}
    if(current_time < 10000) {Serial.print("0");}
    if(current_time < 1000) {Serial.print("0");}
    if(current_time < 100) {Serial.print("0");}
    if(current_time < 10) {Serial.print("0");}
    Serial.print(current_time); Serial.print(" ");

    //Print all of the data
    Serial.print(humidity.relative_humidity); Serial.print(" ");
    
    Serial.print(temp.temperature); Serial.print(" ");
    
  
    Serial.print(archtemp); Serial.print(" ");
    
  	Serial.print(bridgetemp); Serial.print(" ");
  
    Serial.print(heeltemp);
    Serial.println("");
  }

  //Check if changes in data is above threshold or not.
  if(heeltemp > prev_heeltemp + temp_threshold || heeltemp < prev_heeltemp - temp_threshold){
    delay_seconds = initial_delay;
  }
  else if(archtemp > prev_archtemp + temp_threshold || archtemp < prev_archtemp - temp_threshold){
    delay_seconds = initial_delay;
  }
  else if(bridgetemp > prev_bridgetemp + temp_threshold || bridgetemp < prev_bridgetemp - temp_threshold){
    delay_seconds = initial_delay;
  }
  else if(temp.temperature > prev_toetemp + temp_threshold || temp.temperature < prev_toetemp - temp_threshold){
    delay_seconds = initial_delay;
  }
  else if (humidity.relative_humidity > prev_humi + humidity_threshold || humidity.relative_humidity < prev_humi - humidity_threshold){
    delay_seconds = initial_delay;
  } 
  else {
    delay_seconds = min(delay_seconds+delay_step,max_delay);
  }

  //Save all previous temperature readings for reference
  prev_heeltemp = heeltemp;
  prev_archtemp = archtemp;
  prev_bridgetemp = bridgetemp;
  prev_toetemp = temp.temperature;
  prev_humi = humidity.relative_humidity;
  
	delay(delay_seconds*1000);
  current_time = current_time + delay_seconds;


  //Tests on creating a config file to change delays etc. using internal file system: not finished.
  if(Serial.available() > 0){
    entered_password = Serial.read();
    while (Serial.available()){
      //Remove any excess data on the Serial.read buffer
      password_buffer = Serial.read();
    }
    if (password == entered_password){
        Serial.println("\n\nPassword accepted: Entering Config Mode");
        int config_flag = 1;
        int command_number = 0;
        while (config_flag == 1){
          Serial.println("1: Change Initial Delay");
          Serial.println("2: Change Delay Step");
          Serial.println("3: Change Max Delay");
          Serial.println("4: Change Send Flag");
          Serial.println("5: Change Password");
          Serial.println("6: Change Temp Threshold");
          Serial.println("7: Change Humidity Threshold");
          Serial.println("0: Exit\n\n");
          Serial.println("Current config file status: ");
          
          file.open(FILENAME, FILE_O_READ);
          // file existed
          if ( file )
          {
            Serial.println(FILENAME " file exists");
            
            uint32_t readlen;
            char buffer[128] = { 0 };
            readlen = file.read(buffer, sizeof(buffer));
        
            buffer[readlen] = 0;
            Serial.println(buffer);
            Serial.println("");
            Serial.println("");
            file.close();
          } else {
              Serial.println(FILENAME " file does not exist");
          }
              
          while ( !Serial.available() )
          {
              delay(1);
          }
          command_number = Serial.read();
          while (Serial.available()){
            //Remove any excess data on the Serial.read buffer
            password_buffer = Serial.read();
          }
          if (command_number == 48){
            //0 Entered
            config_flag = 0;
          } 
          else if (command_number == 49){
              //1 entered

            }
            else if (command_number == 50){
              //2 entered

            }
          
          
        }
        delay_seconds = initial_delay;
    }
    entered_password = 0;
  }
}
