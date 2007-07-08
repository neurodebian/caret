
#ifndef __VE_BYTE_SWAPPING__
#define __VE_BYTE_SWAPPING__

/// This class contains static methods for byte swapping data, typically used
/// when reading binary data files.
class ByteSwapping {
   public:
      /// byte swap the data
      static void swapBytes(short* n, int numToSwap);
      
      /// byte swap the data
      static void swapBytes(unsigned short* n, int numToSwap);
      
      /// byte swap the data
      static void swapBytes(int* n, int numToSwap);
      
      /// byte swap the data
      static void swapBytes(unsigned int* n, int numToSwap);
      
      /// byte swap the data
      static void swapBytes(long* n, int numToSwap);
      
      /// byte swap the data
      static void swapBytes(unsigned long* n, int numToSwap);
      
      /// byte swap the data
      static void swapBytes(float* n, int numToSwap);
      
      /// byte swap the data
      static void swapBytes(double* n, int numToSwap);
  
};

#endif  // __VE_BYTE_SWAPPING__
