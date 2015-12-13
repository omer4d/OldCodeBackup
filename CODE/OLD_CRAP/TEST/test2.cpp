#include <stdio.h>
#include <conio.h>

#include "Vec2f.hpp"
#include "Mat9f.hpp"




void al_invert_transform(Mat9f *trans)
{
   float det, t;
   
   det =  trans->m[0][0] *  trans->m[1][1] -  trans->m[1][0] *  trans->m[0][1];

   t =  trans->m[2][0];
   trans->m[2][0] = ( trans->m[1][0] *  trans->m[2][1] - t *  trans->m[1][1]) / det;
   trans->m[2][1] = (t *  trans->m[0][1] -  trans->m[0][0] *  trans->m[2][1]) / det;

   t =  trans->m[0][0];
   trans->m[0][0] =  trans->m[1][1] / det;
   trans->m[1][1] = t / det;
   
   trans->m[0][1] = - trans->m[0][1] / det;
   trans->m[1][0] = - trans->m[1][0] / det;
}


int main()
{
 Vec2f src(666, 111);
 Mat9f mat;
 
 mat.initIdentity();
 
 mat.translate(1, 5);
 mat.rotate(0.3);
 mat.translate(2, 5);
 
 mat.translate(55, 124);
 mat.rotate(0.1);
 mat.translate(55, 23);
 
 mat.translate(0, 5);
 mat.rotate(-0.9);
 mat.translate(33, 4);
 
 Vec2f dest = src * mat;
 
 printf("%f %f\n", dest.x, dest.y);
 
 // Backwards:
 
 mat.initIdentity();
 
 mat.translate(-33, -4);
 mat.rotate(0.9);
 mat.translate(-0, -5);
 
 mat.translate(-55, -23);
 mat.rotate(-0.1);
 mat.translate(-55, -124);
 
 mat.translate(-2, -5);
 mat.rotate(-0.3);
 mat.translate(-1, -5);
 
 //al_invert_transform(&mat);
 //Mat9f inv = mat;
 
 Mat9f inv;
 inv.inverseOf(mat);
 //mat.invert();
 Vec2f newDest = src * inv;
 
 //Vec2f newSrc = dest * mat;
 printf("%f %f", newDest.x, newDest.y);
 
 
 /*
  Mat9f mat;
  Vec2f src(222, 111);

  mat.initIdentity();
  mat.rotate(0.1);
  mat.translate(1, 1);

  Vec2f dest = src * mat;

 Mat9f inv;
 inv.inverseOf(mat);

 Vec2f srcNew = dest * inv;

 printf("%f %f", srcNew.x, srcNew.y);*/

 
 getch();
 return 0;
} 

