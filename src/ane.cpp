/*

#include <Arduino.h>
#include <ane.h>


Anemometro::Anemometro(pin_size_t pin, float scala, float Vmin, float Vmax)
{
    _pin = pin;
    _scala = scala;
    pinMode(_pin, INPUT_PULLUP);
    _Vmin = Vmin;
    _Vmax = Vmax;
    // periodo minimo in us, corrispondente alla velocità massima
    _dtMin = 1e6 / (_Vmax / _scala);
    // periodo massimo in us, corrispondente alla velocità minima
    _dtMax = 1e6 / (_Vmin / _scala);
}

// attivazione misure
void Anemometro::On()
{
    // approfondire da: https://forum.arduino.cc/t/attachinterrupt-within-a-class/384024/9
    attachInterrupt(_pin, anemometroISR, RISING);
    _dt = 0;
    _done = false;
}

// arresto misure
void Anemometro::Off()
{
    detachInterrupt(_pin);
    _dt = 0;
    _done = false;
}

// lettura periodo in us
uint32_t Anemometro::Periodo()
{
    if (_done)
    {
        return _dt;
    }
    else
    {
        return 0;
    }
}

// lettura velocità in m/s
float Anemometro::Velocita()
{
    if (_done)
    {
        if (_dt > _dtMax)
        {
            // velocità troppo bassa
            return 0.0;
        } else if(_dt < _dtMin) {
            // velocità troppo alta
            return _Vmax;
        } else {
            // velocità nei limiti
            return (_scala / (float(_dt) / 1e6));
        }
    }
    else
    {
        return 0.0;
    }
}


// ISR anemometro
void Anemometro::anemometroISR()
{
    if (!_done)
    {
        _t1 = micros();
        _done = true;
    }
    else
    {
        _t2 = micros();
        _dt = _t2 - _t1;
        _t1 = _t2;
    }
}
*/