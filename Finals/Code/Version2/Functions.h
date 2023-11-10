/*
 * Simple header file for functions
 */

#ifndef Functions_h
#define Functions_h

#include "Arduino.h"

//function declarations
bool withinTolerance(uint16_t a, uint16_t b, uint16_t tolerance);

uint16_t getMax(uint16_t *arr, uint8_t arr_len);
uint16_t getMin(uint16_t *arr, uint8_t arr_len);

int8_t index(char val, char *arr, uint8_t arr_len);
uint8_t maxIndex(uint16_t *arr, uint8_t arr_len);
uint8_t minIndex(uint16_t *arr, uint8_t arr_len);

String strArray(uint16_t *arr, uint8_t arr_len, bool new_line);

#endif
