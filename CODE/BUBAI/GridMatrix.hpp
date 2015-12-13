#ifndef _GRIDMATRIX_HPP_
#define _GRIDMATRIX_HPP_

#include <cmath>

template <class T>
class GridMatrix {
	     static float JFACTOR; // Affects the vertical spacing between bubbles.
	
	     static const int EVEN_DI[];
	     static const int EVEN_DJ[];
	     static const int ODD_DI[];
	     static const int ODD_DJ[];
	     T** data;
	     
	     void freeData()
	     {
	     	if(!data)
	     		return;
	     	
	     	for(int i = 0; i < colNum; ++i)
              delete[] data[i];
           delete[] data;
	     }
	
	public:
		bool symmetric; // Remove one bubble from odd row indices to make the grid symmetrical?
      	int colNum, rowNum;
      	float cellSize;
      
      	T defVal;
		
      GridMatrix(int pColNum, int pRowNum, float pCellSize, bool pSymmetric, T pDefVal):
		defVal(pDefVal), symmetric(pSymmetric), rowNum(pRowNum), colNum(pColNum), cellSize(pCellSize)
	     {
			this->data = new T*[colNum];
		         
		    for(int i = 0; i < colNum; ++i)
		    	this->data[i] = new T[rowNum];
           
           //this->cellSize = cellSize;
           
           clear(pDefVal);
      }
      
      GridMatrix(GridMatrix const& other):
      	symmetric(other.symmetric), colNum(other.colNum), rowNum(other.rowNum),
		  cellSize(other.cellSize), defVal(other.defVal)
      {	
	  		data = new T*[colNum];
		         
		    for(int i = 0; i < colNum; ++i)
				data[i] = new T[rowNum];
			
			for(int j = 0; j < rowNum; ++j)
				for(int i = 0; i < colNum; ++i)
					data[i][j] = other.data[i][j];
      }
      
      GridMatrix(GridMatrix&& other)
      {
	  		this->cellSize = other.symmetric;
    		this->colNum = other.colNum;
    		this->rowNum = other.rowNum;
    		this->defVal = other.defVal;
    		this->data = other.data;
      		other.data = nullptr;
      }
      
      GridMatrix& operator=(GridMatrix&& other)
      {
    		freeData();
      	 	this->symmetric = other.symmetric;
    	 	this->colNum = other.colNum;
    	 	this->rowNum = other.rowNum;
    	 	this->defVal = other.defVal;
    	 	this->data = other.data;
      	 	other.data = nullptr;
      }
    
    GridMatrix& operator=(GridMatrix const& other)
    {
       GridMatrix tmp(other);
	   *this = std::move(tmp);
	   return *this;	
    }
    
    ~GridMatrix()
      {
      	 freeData();
      }
    
    // ***********
    // * Getters *
    // ***********
	
	T** getData()
	{
	   	return data;
	}
	
	T const* const* getData() const
	{
	   	return data;
	}
	
    int getLogicColNum(int j) const
	{
    	return (symmetric && (j % 2) == 1) ? colNum - 1 : colNum;
    }
	
    float getWidth() const
	{
		return symmetric ? colNum * cellSize : (colNum + 0.5f) * cellSize;
    }
	
	float getHeight() const
	{
		return rowNum * JFACTOR * cellSize;
    }
    
    // ***********
    // * Utility *
    // ***********
    
	T const& at(int i, int j) const
    {
    	if(i < 0 || j < 0 || i > colNum - 1 || j > rowNum - 1)
    		return defVal;
    	else
    		return data[i][j];
    }
    
    T const& wat(float x, float y) const
    {
    	int i = toBx(x, y), j = toBy(y);
    	return at(i, j);
    }
    
    void set(int i, int j, T fresh)
    {
    	if(i < 0 || j < 0 || i > colNum - 1 || j > rowNum - 1)
    		return;
    	
    	data[i][j] = fresh;
    }
    
    void setw(float x, float y, T b)
    {
    	int i = toBx(x, y), j = toBy(y);
    	set(i, j, b);
    }
    
    void clear(T val)
    {
    	for(int j = 0; j < rowNum; ++j)
    		for(int i = 0; i < colNum; ++i)
    			data[i][j] = val;
    }
    
    int clampI(int i) const
    {
    	if(i < 0) return 0;
    	else if(i > colNum - 1) return colNum - 1;
    	else return i;
    }
    
    int clampJ(int j) const
    {
    	if(j < 0) return 0;
    	else if(j > rowNum - 1) return rowNum - 1;
    	else return j;
    }
    
    // **************************
    // * Coordinate conversions *
    // **************************

static int mod2(int n)
{
   int mod = n % 2;
   return mod >= 0 ? mod : 2 + mod;
}

float xoffs(float j) const
	{
		return cellSize * 0.5f * mod2(int(j));
	}
	
float toBy(float wy) const
	{
		return wy / (cellSize * JFACTOR);
	}
	
float toBx(float wx, float wy) const
	{
		int j = (int)toBy(wy);
		return (wx - xoffs(j)) / cellSize;
	}
	
    float toWy(float j) const
    {
          return j * cellSize * JFACTOR + cellSize / 2;
    }

    float toWx(float i, float j) const
    {
          return i * cellSize + cellSize / 2 + xoffs(j);
    }
    
    // *************
    // * Neighbors *
    // *************

static int getNeighborI(int i, int j, int n)
	{
		return mod2(j) == 0 ? i + EVEN_DI[n] : i + ODD_DI[n];
	}
	
static int getNeighborJ(int i, int j, int n)
	{
		return mod2(j) == 0 ? j + EVEN_DJ[n] : j + ODD_DJ[n];
	}
	
T const& getNeighbor(int i, int j, int n) const
	{
		return mod2(j) == 0 ? at(i + EVEN_DI[n], j + EVEN_DJ[n]) : 
							at(i + ODD_DI[n], j + ODD_DJ[n]);
	}
};

template <class T>
float GridMatrix<T>::JFACTOR = (float)sqrt(3.f) * 0.5f; // Affects the vertical spacing between bubbles.

	template <class T>
const int GridMatrix<T>::EVEN_DI[] = {-1, 0, -1, 1, -1, 0};

template <class T>
const int GridMatrix<T>::EVEN_DJ[] = {-1, -1, 0, 0, 1, 1};

template <class T>
const int GridMatrix<T>::ODD_DI[] = {0, 1, -1, 1, 0, 1};

template <class T>
const int GridMatrix<T>::ODD_DJ[] = {-1, -1, 0, 0, 1, 1};

#endif
