#include "CircularArray.hpp"

namespace Audio {

int CircularArray::abs(int n)
{
    return n < 0 ? -n : n;
}

int CircularArray::wrap(int i) const
{
    if(i >= 0) return i % size;
    else return size - 1 - (abs(rot + 1) % size);
}

void CircularArray::init(int pSize, unsigned short pDefVal)
{
    size = pSize;
    rot = 0;
    arr = new unsigned short[size];
    defVal = pDefVal;

    for(int i = 0; i < size; i++)
        arr[i] = defVal;
}

CircularArray::CircularArray()
{
    size = 0;
    rot = 0;
    arr = NULL;
    defVal = 0;
}

CircularArray::CircularArray(int pSize, unsigned short pDefVal)
{
    init(pSize, pDefVal);
}

CircularArray::~CircularArray()
{
    delete[] arr;
}

int CircularArray::getSize() const
{
    return size;
}

int CircularArray::rotate(int n)
{
    int i, dest = wrap(rot - n);

    if(n < 0)
        if(rot < dest)
            for(i = rot; i < dest; i++) arr[i] = defVal;
        else
        {
            for(i = 0; i < dest; i++) arr[i] = defVal;
            for(i = rot; i < size; i++) arr[i] = defVal;
        }

    rot = dest;
}

unsigned short CircularArray::operator[](int index) const
{
    return arr[wrap(rot + index)];
}

unsigned short& CircularArray::operator[](int index)
{
    return arr[wrap(rot + index)];
}

}