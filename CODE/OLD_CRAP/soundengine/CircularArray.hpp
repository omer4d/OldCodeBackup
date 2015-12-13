#ifndef _CircularArray_hpp_
#define _CircularArray_hpp_

#ifndef NULL
#define NULL 0
#endif

namespace Audio {

class CircularArray {
 int size, rot;
 unsigned short* arr;
 unsigned short defVal;

 static int abs(int n);
 int wrap(int i) const;

 public:
 
 void init(int pSize, unsigned short pDefVal = 0);
        
 CircularArray();
 CircularArray(int pSize, unsigned short pDefVal = 0);

 ~CircularArray();

 int getSize() const;
 int rotate(int n);

 unsigned short operator[](int index) const;
 unsigned short& operator[](int index);
};

}

#endif
