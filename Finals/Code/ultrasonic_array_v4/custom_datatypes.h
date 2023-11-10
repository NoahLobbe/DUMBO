/*
 * Custom datatypes
 */

#ifndef custom_datatypes_h
#define custom_datatypes_h

#include "Arduino.h"


/* Structs */

struct uint16_tuple {
   uint16_t x;
   uint16_t y;
};

struct uint16_triple {
   uint16_t x;
   uint16_t y;
   uint16_t z;
};

struct uint16_triple_tuple {
   uint16_tuple tupx;
   uint16_tuple tupy;
   uint16_tuple tupz;
};


/* Function declarations */
String strTriple(uint16_triple triple);







#endif
