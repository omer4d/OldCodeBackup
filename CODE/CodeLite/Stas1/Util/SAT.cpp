#include "SAT.hpp"

    
    SAT& SAT::getInstance()
    {
        if(!instance)
            instance = new SAT();
        return *instance;
    }
    
    void SAT::pushAxisNormal(Vec2f const& v)
    {
        norms.push_back(v);
    }
    
    void SAT::clearAxisNormals()
    {
        norms.clear();
    }
    
    bool SAT::test(Shape const& shape1, Mat9f const& mat1, Shape const& shape2, Mat9f const& mat2)
    {
        for(int i = 0; i < norms.size(); i++)
            if(!Interval::intersect(shape1.project(mat1, norms[i]), shape2.project(mat2, norms[i])))
                return false;
        return true;
    }
    
    bool SAT::test(Shape const& shape1, Mat9f const& mat1,
                     Shape const& shape2, Mat9f const& mat2,
                     Vec2f &mtd, Vec2f& mtdNorm)
    {
        float depth, minDepth = FLT_MAX;
        int index = -1;

        for(int i = 0; i < norms.size(); i++)
        {
            if(Interval::intersect(shape1.project(mat1, norms[i]), shape2.project(mat2, norms[i]), depth))
            {
                if(fabs(depth) < fabs(minDepth))
                {
                    minDepth = depth;
                    index = i;
                }
            }

            else
                return false;
        }
        
        float dir;
        
        if(fabs(minDepth) < 0.001f)
        {
            Interval i1 = shape1.project(mat1, norms[index]);
            Interval i2 = shape2.project(mat2, norms[index]);
            dir = (i1.min - i2.min) < 0 ? -1 : 1;
        }
        
        else
            dir = minDepth < 0 ? -1 : 1;
        
        mtdNorm = norms[index] * dir;
        mtd = norms[index] * minDepth;
        
        return true;
    }

SAT* SAT::instance = 0;