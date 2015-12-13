template <class T>
struct GridMatrix {
	     static float JFACTOR; // Affects the vertical spacing between bubbles.
	
	     static const int EVEN_DI[];
	     static const int EVEN_DJ[];
	     static const int ODD_DI[];
	     static const int ODD_DJ[];
	
      T** data;
	
	     bool symmetric; // Remove one bubble from odd row indices to make the grid symmetrical?
      int colNum, rowNum;
      float cellSize;
      
      T defVal;
	
      GridMatrix(int colNum, int rowNum, float cellSize, bool symmetric, T pDefVal): defVal(pDefVal)
	     {
           this->symmetric = symmetric;
		         this->colNum = colNum;
		         this->rowNum = rowNum;
		         
		         this->data = new T*[colNum];
		         
		         for(int i = 0; i < colNum; ++i)
		           this->data[i] = new T[rowNum];
           
           this->cellSize = cellSize;
           
           clear(pDefVal);
      }
      
      ~GridMatrix()
      {
           for(int i = 0; i < colNum; ++i)
              delete[] data[i];
           delete[] data;
      }
    
    // ***********
    // * Getters *
    // ***********
	
    int getLogicColNum(int j)
	   {
           return (symmetric && (j % 2) == 1) ? colNum - 1 : colNum;
    }
	
    float getWidth()
	   {
		        return symmetric ? colNum * cellSize : (colNum + 0.5f) * cellSize;
    }
	
	   float getHeight()
	   {
		         return rowNum * JFACTOR * cellSize;
    }
    
    // ***********
    // * Utility *
    // ***********
    
	   T at(int i, int j)
    {
    	if(i < 0 || j < 0 || i > colNum - 1 || j > rowNum - 1)
    		return defVal;
    	else
    		return data[i][j];
    }
    
    T wat(float x, float y)
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
    
    int clampI(int i)
    {
    	if(i < 0) return 0;
    	else if(i > colNum - 1) return colNum - 1;
    	else return i;
    }
    
    int clampJ(int j)
    {
    	if(j < 0) return 0;
    	else if(j > rowNum - 1) return rowNum - 1;
    	else return j;
    }
    
    // **************************
    // * Coordinate conversions *
    // **************************
    
float xoffs(int j)
	{
		return cellSize * 0.5f * (j % 2);
	}
	
int toBy(float wy)
	{
		return (int)floor(wy / (cellSize * JFACTOR));
	}
	
int toBx(float wx, float wy)
	{
		int j = toBy(wy);
		return (int)floor((wx - xoffs(j)) / cellSize);
	}
	
    float toWy(int j)
    {
          return j * cellSize * JFACTOR + cellSize / 2;
    }

    float toWx(int i, int j)
    {
          return i * cellSize + cellSize / 2 + xoffs(j);
    }
    
    // *************
    // * Neighbors *
    // *************

static int getNeighborI(int i, int j, int n)
	{
		return j % 2 == 0 ? i + EVEN_DI[n] : i + ODD_DI[n];
	}
	
static int getNeighborJ(int i, int j, int n)
	{
		return j % 2 == 0 ? j + EVEN_DJ[n] : j + ODD_DJ[n];
	}
	
T getNeighbor(int i, int j, int n)
	{
		return j % 2 == 0 ? at(i + EVEN_DI[n], j + EVEN_DJ[n]) : 
							at(i + ODD_DI[n], j + ODD_DJ[n]);
	}
	
	/*
    public void getNeighbors(ArrayList<T> arr, int i, int j)
    {	
    	if(j % 2 == 0)
    	{
    		arr.add(at(i - 1, j - 1));
    		arr.add(at(i, j - 1));
    		arr.add(at(i - 1, j));
    		arr.add(at(i + 1, j));
    		arr.add(at(i - 1, j + 1));
    		arr.add(at(i, j + 1));
    	}
    	
    	else
    	{
    		arr.add(at(i, j - 1));
    		arr.add(at(i + 1, j - 1));
    		arr.add(at(i - 1, j));
    		arr.add(at(i + 1, j));
    		arr.add(at(i, j + 1));
    		arr.add(at(i + 1, j + 1));
    	}
    }*/
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
