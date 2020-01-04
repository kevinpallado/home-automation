void checkThisDevice(String server, int port, String url, String methodq)
{
  JsonObject& root = jsonBuffer.createObject();
  
  HTTPClient http;
  http.setTimeout(5000);
  http.begin(server,port,url);
  http.addHeader("Content-Type", "application/json");

  root["macAddress"] = getMacAddress();
  methodq == "device-state" ? root["accountid"] =  EEPROM.read(0) : root["accountid"] = 0;
  Serial.print("MAC Addr => ");
  Serial.println(getMacAddress());
  String output;
  root.printTo(output);
    
  int registerDevice = 0;

  while(registerDevice < 1)
  {
    Serial.print("Checking device if register => ");
    Serial.println(output);
    int responseCode = http.POST(output);

    if(responseCode > 0)
    {
      String response = http.getString();
      JsonObject& root = jsonBuffer.parseObject(response);
      Serial.println(responseCode);
      Serial.print("Response => ");
      String res = root["register"];
      registerDevice = res.toInt();
      Serial.println(registerDevice);
      if(registerDevice > 0)
      {
        bool init_setup = root["init_setup"];
        if(init_setup)
        {
          String acc_id = root["accountid"];
          String dev_id = root["deviceid"];
          String dev_st = root["state"];
          String dev_ap = root["application"];
          eeprom_write(acc_id,dev_id,dev_st,dev_ap);
          break;
        }
        else
        {
          
          String dev_st = root["state"];
          int dev_stt = dev_st.toInt();

            if(root["application"] == "Appliances"){ check_update_state('A',dev_stt); break;}
            else if(root["application"] == "Window") { check_update_state('W',dev_stt); break;}
            else if(root["application"] == "Door") { check_update_state('D',dev_stt); break;}
          
        }
      }
    }
    else
    {
      Serial.print("Error on sending POST: ");
      Serial.println(responseCode);
      Serial.println("Reconnect to server");
      checkThisDevice(websocket_server,3000,"/automation/devices/event?event=view&method=check", "check");
    }
    delay(1500);
  }
  http.end();
}
String getMacAddress()
{
  String _mac;
  
  WiFi.macAddress(mac);

  for(int i=0;i<6;i++)
  {
    _mac += String(mac[i],HEX);
  }

  return _mac;
}
