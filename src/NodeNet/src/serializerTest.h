//
// Created by  KiviCode on 08/08/2020.
//

#ifndef NODENET_SERIALIZERTEST_H
#define NODENET_SERIALIZERTEST_H


#include <fstream>

class MyRecord
{
public:
    uint8_t x, y;
    float z;

    MyRecord(int x, int y, float z) : x(x),y(y),z(z){}

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( x, y, z );
    }
};

class Wrapper
{
public:
    MyRecord rec, rec2;
    std::string abc;
    Wrapper(MyRecord r, MyRecord r1, std::string abc): rec(r), rec2(r1), abc(abc) {}

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( rec, rec2, abc );
    }
};

#endif //NODENET_SERIALIZERTEST_H
