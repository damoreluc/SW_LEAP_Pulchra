#ifndef _ANEMOMETRO_H
#define _ANEMOMETRO_H
#include <Arduino.h>

// hardware setup
//   pin è il piedino al quale è connessa l'uscita dell'anemometro
//   scala definisce i m/s per Hz, es. 2.44 m/s per Hz
//   Vmin: velocità vento minima, in m/s
//   Vmax: velocità vento massima, in m/s
void anemometroSetup(pin_size_t pin, float scala, float Vmin, float Vmax);

// attivazione misure
void anemometroOn();

// arresto misure
void anemometroOff();

// lettura periodo in us
uint32_t anemometroPeriodo();

// lettura velocità in m/s
float anemometroVelocita();


#endif