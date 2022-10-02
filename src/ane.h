/*
#ifndef _ANEMOMETRO_CLASS_H
#define _ANEMOMETRO_CLASS_H
#include <Arduino.h>

class Anemometro
{
public:
    //   pin è il piedino al quale è connesso l'anemometro
    //   scala definisce i m/s per Hz, es. 2.44 m/s per Hz
    //   Vmin: velocità vento minima, in m/s
    //   Vmax: velocità vento massima, in m/s
    Anemometro(pin_size_t pin, float scala, float Vmin, float Vmax);

    // attivazione misure
    void On();

    // arresto misure
    void Off();

    // lettura periodo in us
    uint32_t Periodo();

    // lettura velocità in m/s
    float Velocita();

private:
    pin_size_t _pin;
    float _scala;
    float _Vmin, _Vmax;
    uint32_t _dtMin, _dtMax;
    volatile bool _done = false;
    volatile uint32_t _t1, _t2, _dt;

    // ISR anemometro
    void anemometroISR();
};

#endif
*/