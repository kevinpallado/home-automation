/***
   0 = Account ID
   1 = Device ID
   2 = Device State
   3 = Device App
*/
boolean eeprom_init()
{
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    return false;
  }
  else
  {
    return true;
  }
}

boolean eeprom_read()
{
  int empty_addr = 0;

  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    int saved_data = byte(EEPROM.read(i));
    saved_data > 0 ? empty_addr : empty_addr++;
  }
  return empty_addr == 4 ? true : false;
}

void eeprom_write(String accid, String devid, String devstate, String devapp)
{
  Serial.println("Writing to EEPROM");
  EEPROM.write(0, accid.toInt());
  EEPROM.write(1, devid.toInt());
  EEPROM.write(2, devstate.toInt());
  if (devapp == "Appliances") {
    EEPROM.write(3, 'A');
  }
  else if (devapp == "Window") {
    EEPROM.write(3, 'W');
  }
  else if (devapp == "Door") {
    EEPROM.write(3, 'D');
  }
  EEPROM.commit();
}


void get_eeprom_data()
{
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    if (i + 1 == EEPROM_SIZE)
    {
      Serial.println(byte(EEPROM.read(i)));
      info[i] = (char*)byte(EEPROM.read(i));
    }
    else
    {
      Serial.println(byte(EEPROM.read(i)));
      info[i] = (char*)byte(EEPROM.read(i));
    }
  }
}
