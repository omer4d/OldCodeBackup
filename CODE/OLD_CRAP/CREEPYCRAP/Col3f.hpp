#ifndef _Col3f_hpp_
#define _Col3f_hpp_

struct Col3f {
      float r, g, b;
      
      static int clamp255(float n)
      {
          if(n > 255) return 255;
          if(n < 0)   return 0;
          return int(n);
      }
      
      void init(float r, float g, float b)
      {
       this->r = r;
       this->g = g;
       this->b = b;
      }
      
      Col3f()
      {
       init(0, 0, 0);
      }
      
      Col3f(float r, float g, float b)
      {
       init(r, g, b);
      }
      
      Col3f& operator+=(Col3f const& other)
      {
       this->r += other.r;
       this->g += other.g;
       this->b += other.b;
       
       return *this;
      }
      
      Col3f& operator-=(Col3f const& other)
      {
       this->r -= other.r;
       this->g -= other.g;
       this->b -= other.b;
       
       return *this;
      }
      
      Col3f& operator*=(float k)
      {
       this->r *= k;
       this->g *= k;
       this->b *= k;
       
       return *this;
      }
      
      Col3f& operator*=(Col3f const& other)
      {
       this->r *= other.r;
       this->g *= other.g;
       this->b *= other.b;
      }
      
      Col3f operator+(Col3f const& other) const
      {
       Col3f temp = *this;
       temp += other;
       
       return temp;
      }
      
      Col3f operator-(Col3f const& other) const
      {
       Col3f temp = *this;
       temp -= other;
       
       return temp;
      }
      
      Col3f operator*(float k) const
      {
       Col3f temp = *this;
       temp *= k;
       return temp;
      }
      
      Col3f operator*(Col3f const& other) const
      {
       Col3f temp = *this;
       temp *= other;
       return temp;
      }
      
      static Col3f gradient(Col3f const& a, Col3f const& b, float k)
      {
          return a + (b - a) * k;
      }
      
      int packBGR()
      {
          return clamp255(b) | (clamp255(g) << 8) | (clamp255(r) << 16);
      }
};

#endif
