#ifndef _Col3f_hpp_
#define _Col3f_hpp_

struct Col3f {
      float r, g, b;
      
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
};

#endif
