void init_lcdkp()
{
  lcd.init();

  lcd.backlight();
  Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
}

systemOverrideScript()
{
  char myKey = customKeypad.getKey();
 
  if (myKey != NULL){
    Serial.print("Key pressed: ");
    Serial.println(myKey);
  }
}
