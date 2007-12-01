
#include "ByteSwapping.h"

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(short* n, int numToSwap)
{
   for (int i = 0; i < numToSwap; i++) {
      char* bytes = (char*)&n[i];
      char  temp = bytes[0];
      bytes[0] = bytes[1];
      bytes[1] = temp;
   }
}

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(unsigned short* n, int numToSwap)
{
   swapBytes((short*)n, numToSwap);
}

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(int* n, int numToSwap)
{
   for (int i = 0; i < numToSwap; i++) {
      char* bytes = (char*)&n[i];
      char  temp = bytes[0];
      bytes[0] = bytes[3];
      bytes[3] = temp;

      temp = bytes[1];
      bytes[1] = bytes[2];
      bytes[2] = temp;
   }
}

/**
 *
 */
void 
ByteSwapping::swapBytes(unsigned int* n, int numToSwap)
{
   swapBytes((int*)n, numToSwap);
}

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(long long* n, int numToSwap)
{
   for (int i = 0; i < numToSwap; i++) {
      char* bytes = (char*)&n[i];
      char  temp = bytes[0];
      bytes[0] = bytes[7];
      bytes[7] = temp;

      temp = bytes[1];
      bytes[1] = bytes[6];
      bytes[6] = temp;

      temp = bytes[2];
      bytes[2] = bytes[5];
      bytes[5] = temp;

      temp = bytes[3];
      bytes[3] = bytes[4];
      bytes[4] = temp;
   }
}

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(unsigned long long* n, int numToSwap)
{
   swapBytes((long long*)n, numToSwap);
}

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(float* n, int numToSwap)
{
   swapBytes((int*)n, numToSwap);
}

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(double* n, int numToSwap)
{
   swapBytes((long long*)n, numToSwap);
}

