// functions.h
#ifdef __cplusplus
extern "C" {
#endif

void buttonTick();

void toggleLampState(bool on);

void toggleLampBri(int bri);

void changeMode(int mode);

extern homekit_characteristic_t led_on;

#ifdef __cplusplus
}
#endif