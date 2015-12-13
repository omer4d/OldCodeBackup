#ifndef _FLOOD_HPP_
#define _FLOOD_HPP_

#include <functional>

#include "GridMatrix.hpp"

template<typename T>
class Flood {
	typedef std::function<bool(T const&, int, int)> Rule;
	typedef std::function<void(T const&, int, int)> Processor;
	
	struct Point {
		int i, j;
		
		Point(int pi, int pj)
  		{
			 i = pi;
			 j = pj;
		}
	};

	int mColNum, mRowNum;
	bool** mMarked;
	GridMatrix<T>* mGrid;

	bool needsProc(int i, int j, Rule const& rule) const
	{
		return !mMarked[i][j] && (!rule || rule(mGrid->at(i, j), i, j));
	}

	int procRow(std::vector<Point>& deque, int i0, int j0, Rule const& rule, Processor const& proc)
	{
		int counter = 0;
		int i, firstI, lastI;
		bool insideRowAbove = false, insideRowBelow = false;

		// Process the current row:

		for (i = i0 - 1; i >= 0 && needsProc(i, j0, rule); --i)
		{
			if(proc)
				proc(mGrid->at(i, j0), i, j0);
			mMarked[i][j0] = true;
			++counter;
		}

		firstI = i + 1;
		
		for (i = i0; i < mColNum && needsProc(i, j0, rule); ++i)
		{
			if(proc)
				proc(mGrid->at(i, j0), i, j0);
			mMarked[i][j0] = true;
			++counter;
		}

		// Adjust the search range for matching rows above/below this one:

		lastI = i - 1;

		if (j0 % 2 == 0)
			--firstI;
		else
			++lastI;

		if (firstI < 0)
			firstI = 0;

		if (lastI > mColNum - 1)
			lastI = mColNum - 1;

		// Add to the deque all matching rows touching the current one:

		for (i = firstI; i <= lastI; ++i)
		{
			if (j0 > 0)
			{
				if (needsProc(i, j0 - 1, rule))
				{
					if (!insideRowAbove)
						deque.push_back(Point(i, j0 - 1));
					insideRowAbove = true;
				}

				else
					insideRowAbove = false;
			}

			if (j0 < mRowNum - 1)
			{
				if (needsProc(i, j0 + 1, rule))
				{
					if (!insideRowBelow)
						deque.push_back(Point(i, j0 + 1));
					insideRowBelow = true;
				}

				else
					insideRowBelow = false;
			}
		}
		
		return counter;
	}

public:
	void reset()
	{
		for (int j = 0; j < mRowNum; ++j)
			for (int i = 0; i < mColNum; ++i)
				mMarked[i][j] = false;
	}

	Flood(GridMatrix<T>* grid):
		mColNum(grid->colNum), mRowNum(grid->rowNum), mGrid(grid)
	{
		mMarked = new bool*[mColNum];
		
  		for(int i = 0; i < mColNum; ++i)
    		mMarked[i] = new bool[mRowNum];
    	
		reset();
	}

	int run(int i0, int j0, bool needReset, Rule const& rule=nullptr, Processor const& proc=nullptr)
	{
		std::vector<Point> deque;
		int counter = 0;

		if(needReset)
			reset();
		
		if (!needsProc(i0, j0, rule))
			return 0;
		
		deque.push_back(Point(i0, j0));
		
		while (deque.size() > 0)
		{
			Point p = deque.back();
			deque.pop_back();
			counter += procRow(deque, p.i, p.j, rule, proc);
		}
		
		return counter;
	}
	
	bool const* const* getMarkMat() const
	{
		return mMarked;
	}
	
	bool getMarkerAt(int i, int j) const
	{
		if(i >= 0 && j >= 0 && i < mColNum && j < mRowNum)
			return mMarked[i][j];
		else
			return false;
	}
};

#endif
