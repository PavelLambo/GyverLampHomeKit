void saveEEPROM() {
  EEPROM.put(EEPROM_START_ADDRESS+(3 * currentMode + 40), modes[currentMode]);
  EEPROM.commit();
}

void eepromTick() {
  if (settChanged && millis() - eepromTimer > 30000) {
    settChanged = false;
    eepromTimer = millis();
    saveEEPROM();
    if (EEPROM.read(EEPROM_START_ADDRESS+200) != currentMode) EEPROM.write(EEPROM_START_ADDRESS+200, currentMode);
    EEPROM.commit();
  }
}

void saveAlarm(byte almNumber) {
  EEPROM.write(EEPROM_START_ADDRESS+(5 * almNumber), alarm[almNumber].state);   // рассвет
  eeWriteInt(5 * almNumber + 1, alarm[almNumber].time);
  EEPROM.commit();
}

void saveDawnMmode() {
  EEPROM.write(EEPROM_START_ADDRESS+199, dawnMode);   // рассвет
  EEPROM.commit();
}