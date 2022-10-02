#include <TMP36.h>

TMP36::TMP36(byte pin, eAnalogReference mode) {
    this->_pin = pin;
    this->_mode = mode;
    analogReference(this->_mode);
    analogReadResolution(12);

    // imposta la tensione di riferimento tra quelle possibili
    switch (this->_mode) {
        case AR_DEFAULT: this->_Vref = 3.3; break;
        case AR_INTERNAL: this->_Vref = 2.23; break;
        case AR_INTERNAL1V0: this->_Vref = 1.0; break; 
        case AR_INTERNAL1V65: this->_Vref = 1.65; break; 
        case AR_INTERNAL2V23: this->_Vref = 2.23; break; 
    default:
        this->_Vref = 3.3;                   
    }
}

float TMP36::Vreference() {
    return this->_Vref;
}

int TMP36::raw() {
    this->_raw = analogRead(this->_pin);
    return this->_raw;
}

float TMP36::temperature() {
    float volt;
    this->_raw = analogRead(this->_pin);
    volt = (float)this->_raw * this->_Vref / 4096.0;
    this->_temperature = (volt - 0.5) * 100.0;
    return this->_temperature;
}