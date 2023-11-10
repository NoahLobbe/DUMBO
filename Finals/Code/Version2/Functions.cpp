/*
 * CPP file for functions
 */


#include "Functions.h"


bool withinTolerance(uint16_t a, uint16_t b, uint16_t tolerance) {
  return (abs(a - b) <= tolerance); //https://www.mathsisfun.com/numbers/absolute-value.html=
}


uint16_t getMax(uint16_t *arr, uint8_t arr_len) {
  uint16_t curr_max = arr[0];
  for (uint8_t i=0; i<arr_len; i++) {
    if (arr[i] > curr_max) {
      curr_max = arr[i];
    }
  }
  return curr_max;
}

uint16_t getMin(uint16_t *arr, uint8_t arr_len) {
  uint16_t curr_min = arr[0];
  for (uint8_t i=0; i<arr_len; i++) {
    if (arr[i] < curr_min) {
      curr_min = arr[i];
    }
  }
  return curr_min;
}


int8_t index(char val, char *arr, uint8_t arr_len) { //find index of a char in an array
  for (uint8_t i=0; i<arr_len; i++) {
    if (val == arr[i]) {
      return i;
    }
  }
  return -1; //couldn't find it :(
}

uint8_t maxIndex(uint16_t *arr, uint8_t arr_len) { //finds the index of the largest item in the array
  uint8_t index = 0;
  uint16_t curr_max = arr[index];
  
  for (uint8_t i=0; i<arr_len; i++) {
    if (arr[i] > curr_max) {
      curr_max = arr[i];
      index = i;
    }
  }
  return index;
}

uint8_t minIndex(uint16_t *arr, uint8_t arr_len) { //finds the index of the smallest item in the array
  uint8_t index = 0;
  uint16_t curr_min = arr[index];
  
  for (uint8_t i=0; i<arr_len; i++) {
    if (arr[i] < curr_min) {
      curr_min = arr[i];
      index = i;
    }
  }
  return index;
}


String strArray(uint16_t *arr, uint8_t arr_len, bool new_line=true) {
  String str = "[";
  for (uint8_t i=0; i<arr_len; i++) {
    str += String(arr[i]) + ((i<arr_len-1) ? ",":"");
  }
  str += "]";
  str += (new_line) ? "]\n" : "]";
  return str;
}
