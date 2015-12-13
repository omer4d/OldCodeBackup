#include <stdio.h>
#include <conio.h>
#include <inttypes.h>

#include <vector>

using namespace std;

struct Test {
   int x;
};

template <class T>
struct Pool {
   union Slot {
      Slot* nextFreeSlot;
      T object;
   };
   
   int size;  
   Slot* space;
   Slot* freeSlot;
   
   Pool(int size)
   {
       this->size = size;
       this->space = new Slot[size];
       this->freeSlot = space;
       
       for(int i = 0; i < size - 1; ++i)
          space[i].nextFreeSlot = &space[i + 1];
       space[size - 1].nextFreeSlot = NULL;
   }
   
   ~Pool()
   {
       delete[] space;
   }
   
   T* alloc()
   {
       T* ptr = &freeSlot->object;
       freeSlot = freeSlot->nextFreeSlot;
       return ptr;
   }
   
   void free(T* object)
   {
       Slot* freed = (Slot*)object;
       freed->nextFreeSlot = freeSlot;
       freeSlot = freed;
   }
};

struct A {
   int data1;
   
   A()
   {
      data1 = 666;
   }
};

struct B: public A {
       int data2;
       
       B()
       {
           data2 = 777;
       }
};

int main()
{
   Pool<Test> pool(10);
   vector<Test*> ptrVec;
   vector<Test> copyVec;
   
   srand(time(NULL));
   
   for(int i = 0; i < 10; ++i)
   {
       Test* t = pool.alloc();
       t->x = i + 1;
       ptrVec.push_back(t);
   }
   
   for(int j = 0; j < 100000; j++)
   {
   
   int n = rand() % (ptrVec.size() + 1);
   
   for(int i = 0; i < n; ++i)
   {
       vector<Test*>::iterator iter = ptrVec.begin() + (rand() % ptrVec.size());
       Test* t = *iter;
       
       copyVec.push_back(*t);
       pool.free(t);
       ptrVec.erase(iter);
  }

  n = rand() % (copyVec.size() + 1);

  for(int i = 0; i < n; ++i)
  {
       vector<Test>::iterator iter = copyVec.begin() + (rand() % copyVec.size());
       Test t = *iter;
       Test* tp = pool.alloc();
       tp->x = t.x;
       
       ptrVec.push_back(tp);
       copyVec.erase(iter);
  }
  }
  
  int n = copyVec.size();

  for(int i = 0; i < n; ++i)
  {
       vector<Test>::iterator iter = copyVec.begin() + (rand() % copyVec.size());
       Test t = *iter;
       Test* tp = pool.alloc();
       tp->x = t.x;
       
       ptrVec.push_back(tp);
       copyVec.erase(iter);
  }
   
   for(int i = 0; i < pool.size; ++i)
      printf("%d ", pool.space[i].object.x);
 
   B b;
   printf("\n\n\n\n%d %d", b.data1, b.data2);
 
   getch();
   return 0;
}
