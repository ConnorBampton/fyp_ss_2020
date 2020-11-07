/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
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





#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery

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

  // Setup the BLE LED to be enabled on CONNECT
  Bluefruit.autoConnLed(true);

  // Config the peripheral connection with maximum bandwidth 
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Bluefruit52");
  //Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();

  // Start BLE Battery Service
  blebas.begin();
  blebas.write(100);

  // Set up and start advertising
  startAdv();

  //Serial.println("Please use Adafruit's Bluefruit LE app to connect in UART mode");
  //Serial.println("Once connected, enter character(s) that you wish to send");
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
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
    if(current_time < 10000)  {Serial.print("0");}
    if(current_time < 1000)   {Serial.print("0");}
    if(current_time < 100)    {Serial.print("0");}
    if(current_time < 10)     {Serial.print("0");}
    Serial.print(current_time); Serial.print(" ");

    //Print all of the data
    Serial.print(humidity.relative_humidity); Serial.print(" ");
    
    Serial.print(temp.temperature); Serial.print(" ");
    
  
    Serial.print(archtemp); Serial.print(" ");
    
    Serial.print(bridgetemp); Serial.print(" ");
  
    Serial.print(heeltemp);
    Serial.println("");
    }
  
  
  // Forward data from HW Serial to BLEUART
  if (ble_flag){
    if(current_time < 100000) {bleuart.print("0");}
    if(current_time < 10000)  {bleuart.print("0");}
    if(current_time < 1000)   {bleuart.print("0");}
    if(current_time < 100)    {bleuart.print("0");}
    if(current_time < 10)     {bleuart.print("0");}
    bleuart.print(current_time); bleuart.print(" ");

    //Print all of the data
    bleuart.print(humidity.relative_humidity); bleuart.print(" ");
    
    bleuart.print(temp.temperature); bleuart.print(" ");
    
  
    bleuart.print(archtemp); bleuart.print(" ");
    
    bleuart.print(bridgetemp); bleuart.print(" ");
  
    bleuart.print(heeltemp);
    bleuart.println("");
    
    //uint8_t buf[64];
    //int count = Serial.readBytes(buf, sizeof(buf));
    //bleuart.write( buf, count );
  }
  // Print out to serial all data received over BLE
  while ( bleuart.available() )
  {
    uint8_t ch;
    ch = (uint8_t) bleuart.read();
    Serial.write(ch);
  }
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  //Serial.print("Connected to ");
  //Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  //Serial.println();
  //Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}
