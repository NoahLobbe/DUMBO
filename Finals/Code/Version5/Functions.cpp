/*
 * CPP file for functions
 */


#include "Functions.h"


bool withinTolerance(uint16_t a, uint16_t b, uint16_t tolerance) {
  return (abs(a - b) <= tolerance); //https://www.mathsisfun.com/numbers/absolute-value.html=
}



int8_t index(char *p_arr, char val, uint8_t array_size) { //find index of a char in an array
  for (uint8_t i=0; i<array_size; i++) {
    if (val == p_arr[i]) {
      return i;
    }
  }
  return -1; //couldn't find it :(
}

uint8_t maxIndex(uint16_t *p_arr, uint8_t array_size) { //finds the index of the largest item in the array
  uint8_t index = 0;
  uint16_t curr_max = p_arr[index];
  
  for (uint8_t i=0; i<array_size; i++) {
    if (p_arr[i] > curr_max) {
      curr_max = p_arr[i];
      index = i;
    }
  }
  return index;
}

uint8_t minIndex(uint16_t *p_arr, uint8_t array_size) { //finds the index of the smallest item in the array
  uint8_t index = 0;
  uint16_t curr_min = p_arr[index];
  
  for (uint8_t i=0; i<array_size; i++) {
    if (p_arr[i] < curr_min) {
      curr_min = p_arr[i];
      index = i;
    }
  }
  return index;
}

uint16_t getMax(uint16_t *p_arr, uint8_t array_size) {
  return p_arr[maxIndex(p_arr, array_size)];
}

uint16_t getMin(uint16_t *p_arr, uint8_t array_size) {
  return p_arr[minIndex(p_arr, array_size)];
}


uint8_t numOccurances(bool *p_arr, bool value, uint8_t array_size) {
  uint8_t counter = 0;
  for (uint8_t i=0; i<array_size; i++) {
    counter += (p_arr[i] == value); //either add 1 or 0, simple! :D
  }
  return counter;
}

bool isAllBool(bool *p_arr, bool value, uint8_t array_size) {
   return (numOccurances(p_arr, value, array_size) ==  array_size);
}

bool contains(bool *p_arr, bool value, uint8_t array_size) {
  for (uint8_t i=0; i<array_size; i++) {
    if (p_arr[i] == value) {
      return true;
    }
  }
  return false; // wasn't in array :(
}

bool containsLessThanEqual(uint16_t *p_arr, uint16_t value, uint8_t array_size) {
  for (uint8_t i=0; i<array_size; i++) {
    if (p_arr[i] <= value) {
      return true;
    }
  }
  return false; 
}

bool containsValueWithinRange(uint16_t *p_arr, uint16_t lower_limit, uint16_t upper_limit, uint8_t array_size) {
  for (uint8_t i=0; i<array_size; i++) {
    if ((p_arr[i] > lower_limit) && (p_arr[i] < upper_limit)) {
      return true;
    }
  }
  return false; 
}

bool allWithinRange(uint16_t *p_arr, uint16_t lower_limit, uint16_t upper_limit, uint8_t array_size) {
  for (uint8_t i=0; i<array_size; i++) {
    if ((p_arr[i] <= lower_limit) && (p_arr[i] >= upper_limit)) {
      return false; //an exqualifying value exists
    }
  }
  return true; 
}



void copyArray(uint16_t *p_original_array, uint16_t *p_array_dest, uint8_t array_size) {
  for (uint8_t i=0; i<array_size; i++) {
    p_array_dest[i] = p_original_array[i];
  }
}



String strArray(uint16_t *p_arr, uint8_t array_size, bool new_line=true) {
  String str = "[";
  for (uint8_t i=0; i<array_size; i++) {
    str += String(p_arr[i]) + ((i<array_size-1) ? ",":"");
  }
  str += (new_line) ? "]\n" : "]";
  return str;
}

String strArray(bool *p_arr, uint8_t array_size, bool new_line=true) {
  String str = "[";
  for (uint8_t i=0; i<array_size; i++) {
    str += String(p_arr[i]) + ((i<array_size-1) ? ",":"");
  }
  str += (new_line) ? "]\n" : "]";
  return str;
}
