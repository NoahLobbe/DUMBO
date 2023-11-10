/*
 * 
 */


#include "CustomDataTypes.h"


String strTriple(uint16_triple triple) {
  return (String)("(" + String(triple.x) + "," + String(triple.y) + "," + String(triple.z) + ")");
}
