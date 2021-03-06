#ifndef _Mat9f_hpp_
#define _Mat9f_hpp_

struct Mat9f {
      float m[3][3];
      
      float& operator()(int i, int j)
      {
       return m[i][j];
      }
      
      float const& operator()(int i, int j) const
      {
       return m[i][j];
      }
      
      void initIdentity()
      {
       m[0][0] = 1; m[1][0] = 0; m[2][0] = 0;
       m[0][1] = 0; m[1][1] = 1; m[2][1] = 0;
       m[0][2] = 0; m[1][2] = 0; m[2][2] = 1;
      }
      
      void initScale(float sx, float sy)
      {
       m[0][0] = sx; m[1][0] = 0;  m[2][0] = 0;
       m[0][1] = 0;  m[1][1] = sy; m[2][1] = 0;
       m[0][2] = 0;  m[1][2] = 0;  m[2][2] = 1;
      }
      
      void initTranslation(float tx, float ty)
      {
       m[0][0] = 1; m[1][0] = 0; m[2][0] = tx;
       m[0][1] = 0; m[1][1] = 1; m[2][1] = ty;
       m[0][2] = 0; m[1][2] = 0; m[2][2] = 1;
      }
      
      void initRotation(float a)
      {
       m[0][0] = cos(a); m[1][0] = -sin(a); m[2][0] = 0;
       m[0][1] = sin(a); m[1][1] = cos(a);  m[2][1] = 0;
       m[0][2] = 0;      m[1][2] = 0;       m[2][2] = 1;
      }
      
      void initSwapXy()
      {
       m[0][0] = 0; m[1][0] = 1; m[2][0] = 0;
       m[0][1] = 1; m[1][1] = 0; m[2][2] = 0;
       m[0][2] = 0; m[1][2] = 0; m[2][2] = 1;
      }
      
      Mat9f()
      {
       initIdentity();
      }
      
      Mat9f operator*(Mat9f const& other) const
      {
       Mat9f result;
            
       for(int j = 0; j < 3; j++)
        for(int i = 0; i < 3; i++)
         result.m[i][j] = this->m[0][j] * other.m[i][0] +
                          this->m[1][j] * other.m[i][1] +
                          this->m[2][j] * other.m[i][2];
                               
       return result;
      }
      
      Mat9f& operator*=(Mat9f const& other)
      {
       *this = *this * other;
       return *this;
      }
      
      Mat9f& scale(float sx, float sy)
      {
       Mat9f other;
       
       other.initScale(sx, sy);
       *this *= other;
       
       return *this;
      }
      
      Mat9f& translate(float tx, float ty)
      {
       Mat9f other;
       
       other.initTranslation(tx, ty);
       *this *= other;
       
       return *this;
      }
      
      Mat9f& rotate(float a)
      {
       Mat9f other;
       
       other.initRotation(a);
       *this *= other;
       
       return *this;
      }
      
      Mat9f& swapXy()
      {
       Mat9f other;
       
       other.initSwapXy();
       *this *= other;
       
       return *this;
      }
};

#endif
