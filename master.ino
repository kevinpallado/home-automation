#include <WiFi.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "EEPROM.h"
#include "EmonLib.h"

#define EEPROM_SIZE 4

const char* ssid = "PLDTHOMEDSL9NLOL";
const char* password = "PLDTWIFIqYdaT";
const char* websocket_server = "192.168.1.9";
const char* info[4];

const int relay_pin = 2;
const int buttonOverride = 4;

const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns

char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

float emon_data[5];
float timeRendered;

bool connected;
bool startingChar = false;

byte mac[6];
byte rowPins[ROWS] = {35, 32, 33, 25}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 27, 13}; //connect to the column pinouts of the keypad

EnergyMonitor emon1;

DynamicJsonBuffer jsonBuffer(128);

using namespace websockets;

WebsocketsClient client;

LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup()
{
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  pinMode(relay_pin, OUTPUT);
  init_lcdkp();
  init_emon();
  bool eeprom_ev = eeprom_init();
  if (eeprom_ev)
  {
    lcd.setCursor(0,0);
    lcd.print("Initializing..");
    
    boolean empty_addr = eeprom_read();
    Serial.print("Boolean => ");
    Serial.println(empty_addr);
    if (empty_addr)
    {
      Serial.println("Empty address");
      for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++)
      {
        Serial.print(".");
        delay(1000);
      }
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("Wifi connected");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Wifi Connected");
        checkThisDevice(websocket_server, 3000, "/automation/devices/event?event=view&method=check", "check");
      }
      else
      {
        Serial.println("Not connected to WiFi");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("No wifi");
      }
    }

    else
    {
      Serial.println("Reading eeprom data");
      get_eeprom_data();
      delay(2000);
      init_dev_state();
    }
  }
}

void init_dev_state() // check device state
{
  /***
     byte
     A = 65
     D = 68
     W = 87
  */
  int device_app = byte(EEPROM.read(3));
  int device_state = byte(EEPROM.read(2));
  Serial.print(byte(EEPROM.read(3)));
  Serial.println(" <= device app");
  if (device_app == 68)
  {
    Serial.println("Device app => Door");
    device_state > 0 ? digitalWrite(relay_pin, HIGH) : digitalWrite(relay_pin, LOW);
  }
  else if (device_app == 65)
  {
    Serial.println("Device app => Appliances");
    device_state > 0 ? digitalWrite(relay_pin, HIGH) : digitalWrite(relay_pin, LOW);
  }
}

String check_device_data() // data sender
{
  /***
     byte
     A = 65
     D = 68
     W = 87
  */
  JsonObject& root = jsonBuffer.createObject();
  
  int device_app = byte(EEPROM.read(3));
  int device_state = byte(EEPROM.read(2));
  switch (device_app) {
    case 65:
    {
      read_volt_cur();

      float power = calculate_power();
      float e_usage = energy_usage(timeRendered);
      float e_cost = energy_cost(e_usage, timeRendered);
    
      display_data(power, e_usage, e_cost);

      root["power"] = power;
      root["e_usage"] = e_usage;
      root["e_cost"] = e_cost;
      root["device"] = "Appliances";
      root["state"] = device_state;
      
      break;
    }
    case 68:
    {
      root["device"] = "Door";
      root["state"] = device_state;
      
      break;
    }
    case 87:
    {
      root["device"] = "Window";
      root["state"] = device_state;
      break;
    }
    default:
      break;
  }

  String output;
  root.printTo(output);
  return output;
}

void check_update_state(char d_app, int d_state) // check any updates of the devices from device app into device state
{
  /***
     byte
     A = 65
     D = 68
     W = 87
   0 = Account ID
   1 = Device ID
   2 = Device State
   3 = Device App
  */

  EEPROM.commit();
  int device_state = byte(EEPROM.read(2));
  char device_app = byte(EEPROM.read(3));
  switch (d_app) {
    case 'A':
    {
      if(device_app != 65) EEPROM.write(3,d_app);
      if(device_state != d_state) EEPROM.write(2, d_state);
      break;
    }
    case 'D':
    {
      if(device_app != 68) EEPROM.write(3,d_app);
      if(device_state != d_state)  EEPROM.write(2, d_state);
      break;
    }
    case 'W':
    {
      if(device_app != 87) EEPROM.write(3,d_app);
      if(device_state != d_state) EEPROM.write(2, d_state);
      break;
    }
    default:
      break;
  }
  init_dev_state();
}

void loop() 
{  
  delay(150);
  
  if (WiFi.status() == WL_CONNECTED)
  {
    int overrideState = digitalRead(buttonOverride);
    if(overrideState == HIGH)
    {
      systemOverrideScript();
    }
    else
    {
      checkThisDevice(websocket_server, 3000, "/automation/devices/event?event=view&method=device-state", "device-state");
      int device_state = byte(EEPROM.read(2));
      if(device_state > 0)
      {
        String data_col = check_device_data();
        Serial.println(data_col);
        Serial.println("wifi connected");
        client.poll();
        client.send(data_col);
      }
    }
  }
}
