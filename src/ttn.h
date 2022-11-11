#ifndef _TTN_H
#define _TTN_H

#include <Arduino.h>
/*
  supporto per la codifica dei dati di misura verso TheThingsNetwork
*/

// slot del data packet occupato da ciascuna misura
// NOTA: per aggiungere altri slot, lasciare "last" come ultimo elemento dell'enum
typedef enum {eLM35=0, eRTD, last} eSlot;

#define PKTSIZE (last*2)

void encode( char buffer[], float misura, eSlot slot);

#endif