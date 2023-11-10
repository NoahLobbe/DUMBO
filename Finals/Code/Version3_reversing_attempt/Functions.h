/*
 * Simple header file for functions
 */

#ifndef Functions_h
#define Functions_h

#include "Arduino.h"

////function declarations
//miscenalious
bool withinTolerance(uint16_t a, uint16_t b, uint16_t tolerance);

//array element(s) related
int8_t index(char *p_arr, char value, uint8_t array_size);
uint8_t maxIndex(uint16_t *p_arr, uint8_t array_size);
uint8_t minIndex(uint16_t *p_arr, uint8_t array_size);
uint16_t getMax(uint16_t *p_arr, uint8_t array_size);
uint16_t getMin(uint16_t *p_arr, uint8_t array_size);

uint8_t numOccurances(bool *p_arr, bool value, uint8_t array_size);
bool isAllBool(bool *p_arr, bool value, uint8_t array_size);

bool doesContain(bool *p_arr, bool value, uint8_t array_size);
bool containsLessThanEqual(uint16_t *p_arr, uint16_t value, uint8_t array_size);
bool containsGreaterThanEqual(uint16_t *p_arr, uint16_t value, uint8_t array_size);

bool containsValueWithinRange(uint16_t *p_arr, uint16_t lower_limit, uint16_t upper_limit, uint8_t array_size);
bool allWithinRange(uint16_t *p_arr, uint16_t lower_limit, uint16_t upper_limit, uint8_t array_size);


//array manipulations
void copyArray(uint16_t *original_array, uint16_t *array_dest, uint8_t array_size);

String strArray(uint16_t *p_arr, uint8_t array_size, bool new_line);
String strArray(bool *p_arr, uint8_t array_size, bool new_line);





#endif
