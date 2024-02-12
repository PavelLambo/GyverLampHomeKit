/*
 * simple_led_accessory.c
 * Define the accessory in pure C language using the Macro in characteristics.h
 *
 *  Created on: 2020-02-08
 *      Author: Mixiaoxiao (Wang Bin)
 */

#include <Arduino.h>
#include <homekit/types.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <stdio.h>
#include <port.h>
//#include "globals.h"

//const char * buildTime = __DATE__ " " __TIME__ " GMT";

#define ACCESSORY_NAME  ("GyverLamp")
#define ACCESSORY_SN  ("SN_0123456")  //SERIAL_NUMBER
#define ACCESSORY_MANUFACTURER ("Arduino Homekit")
#define ACCESSORY_MODEL  ("ESP8266")

//#define PIN_LED  2//D4
#define BI_LED_PIN 16//D0

extern bool ONflag;
extern bool sendSettings_flag;

int led_bri = 100; //[0, 100]
float currentSaturation = 100.0;
// bool led_power = false; //true or flase
extern bool ONflag;
extern int currentMode;
//extern int led_bri;
int PWMRANGE = 1000;
#include "func.h"



#ifdef __cplusplus
extern "C" {
#endif

void changeLampStateFromHomeKit(bool newState) {
    toggleLampState(newState); // Вызываем функцию toggle, определенную в .ino
}

void changeLampBriFromHomeKit(int bri) {
    toggleLampBri(bri); // Вызываем функцию toggle, определенную в .ino
}

#ifdef __cplusplus
}
#endif

homekit_value_t led_on_get() {
	return HOMEKIT_BOOL(ONflag);
}

homekit_value_t light_bri_get() {
	return HOMEKIT_INT(led_bri);
}

homekit_value_t led_hue_get() {
	return HOMEKIT_FLOAT((float)currentMode);
}

void led_update() {
	if (ONflag) {
		int pwm = PWMRANGE - (int) (led_bri * 1.0 * PWMRANGE / 100.0 + 0.5f);
		//analogWrite(PIN_LED, pwm);
    digitalWrite(BI_LED_PIN, LOW);
    changeLampStateFromHomeKit(true);
		printf("ON  %3d (pwm: %4d of %d)\n", led_bri, pwm, PWMRANGE);
	} else {
    changeLampStateFromHomeKit(false);
		printf("OFF\n");
		digitalWrite(BI_LED_PIN, HIGH);
	}
}

void led_bri_set(homekit_value_t value) {
	if (value.format != homekit_format_int) {
        return;
    }
  int newBri = value.int_value; // Получаем новое значение яркости
  changeLampBriFromHomeKit(newBri); // Обновляем яркость лампы
}

void led_on_set(homekit_value_t value) {
    printf("led_on_set begin: %d \n", value.bool_value);

    if (value.format != homekit_format_bool) {
        printf("Invalid on-value format: %d\n", value.format);
        return;
    }

    if (value.bool_value) {
        changeLampStateFromHomeKit(true); // Включаем лампу
        printf("HK ON\n");
    } else {
        changeLampStateFromHomeKit(false); // Выключаем лампу
        printf("HK OFF\n");
    }

    printf("led_on_set end \n");
}

homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, ACCESSORY_NAME);
homekit_characteristic_t serial_number = HOMEKIT_CHARACTERISTIC_(SERIAL_NUMBER, ACCESSORY_SN);
homekit_characteristic_t occupancy_detected = HOMEKIT_CHARACTERISTIC_(OCCUPANCY_DETECTED, 0);
homekit_characteristic_t led_on = HOMEKIT_CHARACTERISTIC_(ON, false,
		//.callback=HOMEKIT_CHARACTERISTIC_CALLBACK(switch_on_callback),
		.getter=led_on_get,
		.setter=led_on_set
);


// void led_toggle() {
// 	led_on.value.bool_value = !led_on.value.bool_value;
// 	led_on.setter(led_on.value);
// 	homekit_characteristic_notify(&led_on, led_on.value);
// }

void led_hue_set(homekit_value_t value) {
    if (value.format == homekit_format_float) {
        float hue = value.float_value;
        if (hue >= 0 && hue < 360) {
            int mode = (int)(hue / (360.0 / 19)); // Вычисляем режим на основе позиции в диапазоне
            changeMode(mode); // Вызываем функцию для смены режима
        }
    }
}

homekit_value_t led_saturation_get() {
    return HOMEKIT_FLOAT(currentSaturation);
}

void led_saturation_set(homekit_value_t value) {
    if (value.format == homekit_format_float) {
        currentSaturation = value.float_value;
        // Здесь можно добавить код для обновления состояния устройства с новым значением насыщенности
        // Например, обновление цвета светодиода с использованием FastLED или аналогичной библиотеки
        //updateLedColor(currentHue, currentSaturation);
    }
}

void accessory_identify(homekit_value_t _value) {
	printf("accessory identify\n");
	for (int j = 0; j < 3; j++) {
		// led_power = true;
		//led_update();
		delay(100);
		// led_power = false;
		//led_update();
		delay(100);
	}
}

homekit_accessory_t *accessories[] =
		{
				HOMEKIT_ACCESSORY(
						.id = 1,
						.category = homekit_accessory_category_lightbulb,
						.services=(homekit_service_t*[]){
						HOMEKIT_SERVICE(ACCESSORY_INFORMATION,
                .characteristics=(homekit_characteristic_t*[]){
                &name,
                HOMEKIT_CHARACTERISTIC(MANUFACTURER, ACCESSORY_MANUFACTURER),
                &serial_number,
                HOMEKIT_CHARACTERISTIC(MODEL, ACCESSORY_MODEL),
                HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.0.9"),
                HOMEKIT_CHARACTERISTIC(IDENTIFY, accessory_identify),
                NULL
                }),
						HOMEKIT_SERVICE(LIGHTBULB, .primary=true,
                .characteristics=(homekit_characteristic_t*[]){
                HOMEKIT_CHARACTERISTIC(NAME, "Led"),
                &led_on,
                HOMEKIT_CHARACTERISTIC(BRIGHTNESS, 100, .getter=light_bri_get, .setter=led_bri_set),
                // Добавление характеристик цвета
                HOMEKIT_CHARACTERISTIC(HUE, 0, .getter=led_hue_get, .setter=led_hue_set),
                HOMEKIT_CHARACTERISTIC(SATURATION, 0, .getter=led_saturation_get, .setter=led_saturation_set),
                NULL
                }),
						// HOMEKIT_SERVICE(OCCUPANCY_SENSOR, .primary=false, .characteristics=(homekit_characteristic_t*[]) {
            //     HOMEKIT_CHARACTERISTIC(NAME, "Occupancy"),
            //     &occupancy_detected,
            //     NULL
            //     }),
						NULL
						}),
				NULL
		};

homekit_server_config_t config = {
		.accessories = accessories,
		.password = "111-11-111",
		//.on_event = on_homekit_event,
		.setupId = "ABC2"
};

// homekit_accessory_t *accessories[] =
// 		{
// 				HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_lightbulb, .services=(homekit_service_t*[]){
//               HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]){
//                   &name,
//                   HOMEKIT_CHARACTERISTIC(MANUFACTURER, ACCESSORY_MANUFACTURER),
//                   &serial_number,
//                   HOMEKIT_CHARACTERISTIC(MODEL, ACCESSORY_MODEL),
//                   HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.0.9"),
//                   HOMEKIT_CHARACTERISTIC(IDENTIFY, accessory_identify),
//                   NULL
// 						}),
// 						&lightbulb_service,
//             //&switch_service,
// 				    NULL
//         }),
//         NULL
// 		};

// homekit_server_config_t config = {
// 		.accessories = accessories,
// 		.password = "111-11-111",
// 		//.on_event = on_homekit_event,
// 		.setupId = "ABC1"
// };

void accessory_init() {
	pinMode(BI_LED_PIN, OUTPUT);
	//led_update();
}

