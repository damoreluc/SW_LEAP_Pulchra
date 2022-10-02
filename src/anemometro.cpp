
#include <anemometro.h>

pin_size_t _pin;
float _scala;
float _Vmin, _Vmax;
uint32_t _dtMin, _dtMax;
volatile bool _done = false;
volatile uint32_t _t1, _t2, _dt;

// ISR anemometro
void anemometroISR()
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

// hardware setup
void anemometroSetup(pin_size_t pin, float scala, float Vmin, float Vmax)
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
void anemometroOn()
{
    attachInterrupt(digitalPinToInterrupt(_pin), anemometroISR, RISING);
    _dt = 0;
    _done = false;
}

// arresto misure
void anemometroOff()
{
    detachInterrupt(digitalPinToInterrupt(_pin));
    _dt = 0;
    _done = false;
}

// lettura periodo in us
uint32_t anemometroPeriodo()
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
float anemometroVelocita()
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