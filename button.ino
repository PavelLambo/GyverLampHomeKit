#include "func.h"

boolean brightDirection;

void toggleLampState(bool on) {
    if (dawnFlag) {
        manualOff = true;
        dawnFlag = false;
        loadingFlag = true;
    } 

    ONflag = on;

    if (!ONflag) {
        // FastLED.clear(); // Чистим все светодиоды
        // FastLED.show(); // Применяем изменения
        changePower(); // Включаем с необходимой яркостью
        Serial.println("Turn OFF lamp");
    } else {
        changePower(); // Включаем с необходимой яркостью
        Serial.println("Turn ON lamp");
    }
    
    sendSettings_flag = true;
}


void changeMode(int mode) {
    // Проверка, что mode в допустимом диапазоне
    if (mode >= 0 && mode < MODE_AMOUNT) {
        currentMode = mode; // Устанавливаем новый режим

        // Применяем настройки нового режима
        FastLED.setBrightness(modes[currentMode].brightness);
        loadingFlag = true; // Возможно, индикатор того, что происходит загрузка или переключение режима
        settChanged = true; // Флаг, указывающий на то, что настройки изменились и возможно их нужно сохранить
        eepromTimer = millis(); // Обновляем таймер для отложенного сохранения настроек в EEPROM
        FastLED.clear(); // Очищаем светодиоды перед применением нового режима
        delay(1); // Краткая задержка перед показом нового режима
        sendSettings_flag = true; // Флаг, который может использоваться для отправки обновлённых настроек в другие части программы или устройства

        Serial.print("Mode changed to: ");
        Serial.println(currentMode);
    }
}

void toggleLampBri(int bri) {
    if (ONflag) {
        modes[currentMode].brightness = bri;
        FastLED.setBrightness(modes[currentMode].brightness);
        Serial.print("Set brightness to ");
        Serial.println(modes[currentMode].brightness);
        settChanged = true;
        eepromTimer = millis();
        sendSettings_flag = true;
    }
}

void buttonTick() {
  touch.tick();
  if (touch.isSingle()) {
    if (dawnFlag) {
      manualOff = true;
      dawnFlag = false;
      loadingFlag = true;
      FastLED.setBrightness(modes[currentMode].brightness);
      changePower();
    } else {
      if (ONflag) {
        ONflag = false;
        changePower();
        homekit_characteristic_notify(&led_on, HOMEKIT_BOOL(ONflag));
        Serial.println("Turn OFF lamp");
      } else {
        ONflag = true;
        changePower();
        homekit_characteristic_notify(&led_on, HOMEKIT_BOOL(ONflag));
        Serial.println("Turn ON lamp");
      }
      sendSettings_flag = true;
    }
  }

  if (ONflag && touch.isDouble()) {
    if (++currentMode >= MODE_AMOUNT) currentMode = 0;
    FastLED.setBrightness(modes[currentMode].brightness);
    loadingFlag = true;
    settChanged = true;
    eepromTimer = millis();
    FastLED.clear();
    delay(1);
    sendSettings_flag = true;
  }
  if (ONflag && touch.isTriple()) {
    if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
    FastLED.setBrightness(modes[currentMode].brightness);
    loadingFlag = true;
    settChanged = true;
    eepromTimer = millis();
    FastLED.clear();
    delay(1);
    sendSettings_flag = true;
  }

  // вывод IP на лампу
  if (ONflag && touch.hasClicks()) {
    if (touch.getClicks() == 5) {
      resetString();
      while (!fillString(lampIP, CRGB::Green, true)) {
        delay(1);
        ESP.wdtFeed();   // пнуть собаку
        yield();  // ещё раз пнуть собаку
      }
    }
  }

  if (ONflag && touch.isHolded()) {
    brightDirection = !brightDirection;
  }
  if (ONflag && touch.isStep()) {
    if (brightDirection) {
      if (modes[currentMode].brightness < 10) modes[currentMode].brightness += 1;
      else if (modes[currentMode].brightness < 250) modes[currentMode].brightness += 5;
      else modes[currentMode].brightness = 255;
    } else {
      if (modes[currentMode].brightness > 15) modes[currentMode].brightness -= 5;
      else if (modes[currentMode].brightness > 1) modes[currentMode].brightness -= 1;
      else modes[currentMode].brightness = 1;
    }
    FastLED.setBrightness(modes[currentMode].brightness);
    settChanged = true;
    eepromTimer = millis();
    sendSettings_flag = true;
  }
}