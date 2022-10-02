#ifndef _LM35_H
#define _LM35_H

#include <Arduino.h>

// classe per gestire un sensore LM35
class LM35 {

private:
    byte _pin;
    int _raw;
    float _temperature;
    eAnalogReference _mode;
    float _Vref;

public:
    LM35(byte pin, eAnalogReference mode);
    float Vreference();
    int raw();
    float temperature();

};

#endif