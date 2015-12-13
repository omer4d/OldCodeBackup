#ifndef _GENERATOR_HPP_
#define _GENERATOR_HPP_

#define TYPENUM 7

#define EMPTY -1
#define FULL 1
#define UNVISITED 0

#include <vector>
#include "GridMatrix.hpp"
#include "Util.hpp"

void skeleton(GridMatrix<int>& mat, float ftop, float f0, float fmax, float df)
{
    mat.clear(UNVISITED);
    
    int** data = mat.getData();

    for(int i = 0; i < (mat.getLogicColNum(0) + 1) / 2; ++i)
       data[i][0] = Util::randf() < ftop ? FULL : EMPTY;
    
    for(int j = 0; j < mat.rowNum; ++j)
    {
      f0 += df;
     
      if(f0 > fmax)
        f0 = fmax;
     
      for(int i = 0; i < (mat.getLogicColNum(j) + 1) / 2; ++i)
      {
        if(data[i][j] == UNVISITED)
        {
            for(int k = 0; k < 6; ++k)
               if(mat.getNeighbor(i, j, k) == FULL)
               {           
                  data[i][j] = Util::randf() < f0 ? FULL : EMPTY;
                  break;
               }
        }
      }
    }
}

void symmetrize(GridMatrix<int>& mat)
{
	int** data = mat.getData();
	
   for(int j = 0; j < mat.rowNum; ++j)
      for(int i = 0; i < mat.getLogicColNum(j) / 2; ++i)
         data[mat.getLogicColNum(j) - i - 1][j] = data[i][j];
}

void normalize(std::vector<float>& vec)
{
   float sum = 0.f;
   int n = vec.size();
   
   for(int i = 0; i < n; ++i)
       sum += vec.at(i);
   
   for(int i = 0; i < n; ++i)
      vec.at(i) /= sum;
}

int findFirstLarger(std::vector<float> const& arr, float val)
{
	for(int i = 0; i < arr.size(); ++i)
		if(arr.at(i) > val)
			return i;
	
	return arr.size();
}

int weightedRand(std::vector<float> weights)
{
	normalize(weights);
	
	float accum = 0;
		
	for(int i = 0; i < weights.size(); ++i)
	{
		float temp = weights.at(i);
		weights.at(i) += accum;
		accum += temp;
	}
	
	float f = Util::randf();
	return findFirstLarger(weights, f);
}


void colorize(GridMatrix<int>& result, GridMatrix<int> const& skeleton, std::vector<float> const& baseWeights, float clumping)
{
	std::vector<float> weights = baseWeights;
	
	result.clear(0);
	  
	for(int j = 0; j < skeleton.rowNum; ++j)
		for(int i = 0; i < skeleton.getLogicColNum(j); ++i)
			if(skeleton.at(i, j) == FULL)
			{
				for(int k = 0; k < weights.size(); ++k)
					weights.at(k) = baseWeights.at(k);
				
				for(int k = 0; k < 6; ++k)
				{
					int neigh = result.getNeighbor(i, j, k);
						
					if(neigh > 0)
						weights.at(result.getNeighbor(i, j, k) - 1) *= clumping;
				}
	        
				result.set(i, j, weightedRand(weights) + 1);
			}
}

int count(GridMatrix<int> const& grid)
{
	int counter = 0;
	
	for(int j = 0; j < grid.rowNum; ++j)
		for(int i = 0; i < grid.getLogicColNum(j); ++i)
			if(grid.at(i, j) != 0)
				++counter;
				
	return counter;
}


#endif
