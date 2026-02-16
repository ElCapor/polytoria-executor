#ifndef ARRAYSEGMENT_H
#define ARRAYSEGMENT_H

#include <unity/unity.h>
struct ArraySegment {
    UnityArray<uint8_t>* array;
    int offset;
    int count;
};

#endif /* ARRAYSEGMENT_H */
