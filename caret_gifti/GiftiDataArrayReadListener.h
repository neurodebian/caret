#ifndef __GIFTI_DATA_ARRAY_READ_LISTENER_H__
#define __GIFTI_DATA_ARRAY_READ_LISTENER_H__

#include <QString>

class GiftiDataArray;

/**
 * Classes should extend this class to get data
 * arrays as they are read.
 */
class GiftiDataArrayReadListener
{
public:
    GiftiDataArrayReadListener() { }

    /// constructor
    virtual ~GiftiDataArrayReadListener() { }

    /// returns non-empty string if there is an error
    virtual QString dataArrayWasRead(GiftiDataArray* gda,
                          const int dataArrayIndex,
                          const int numberOfDataArrays) = 0;
};

#endif // __GIFTI_DATA_ARRAY_ADDED_LISTENER_H__

