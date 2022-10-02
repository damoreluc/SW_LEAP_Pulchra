#ifndef _TMP36_H
#define _TMP36_H

#include <Arduino.h>

// classe per gestire un sensore TMP36
class TMP36 {

private:
    byte _pin;
    int _raw;
    float _temperature;
    eAnalogReference _mode;
    float _Vref;

public:
    TMP36(byte pin, eAnalogReference mode);
    float Vreference();
    int raw();
    float temperature();

};

#endif