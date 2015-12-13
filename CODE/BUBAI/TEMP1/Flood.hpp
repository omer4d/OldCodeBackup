#ifndef _FLOOD_HPP_
#define _FLOOD_HPP_

#include "GridMatrix.hpp"

template<typename T>
class Flood {

	public: struct Point {
		int i, j;

		Point(int pi, int pj)
  {
			 i = pi;
			 j = pj;
		}
	}

	public: struct Rule { bool job(T bub, int i, int j); }
	public: struct Processor { void job(T bub, int i, int j); }

	//

	private: int mColNum, mRowNum;
	private: bool mMarked[][];
	private: GridMatrix<T>* mGrid;

	public: void reset()
	{
		for (int j = 0; j < mRowNum; ++j)
			for (int i = 0; i < mColNum; ++i)
				mMarked[i][j] = false;
	}

	public: Flood(GridMatrix<T>* grid)
	{
		mColNum = grid.colNum;
		mRowNum = grid.rowNum;

		mGrid = grid;
		mMarked = new bool[mColNum];
  
  for(int i = 0; i < mColNum; ++i
    mMarked[i] = new bool [mRowNum];

		reset();
	}

/*
	private boolean needsProc(int i, int j, Rule<T> rule)
	{
		return !mMarked[i][j] && (rule == null || rule.job(mGrid.at(i, j), i, j));
	}

	private int procRow(Array<Point> deque, int i0, int j0, Rule<T> rule, Processor<T> proc)
	{
		int counter = 0;
		int i, firstI, lastI;
		boolean insideRowAbove = false, insideRowBelow = false;

		// Process the current row:

		for (i = i0 - 1; i >= 0 && needsProc(i, j0, rule); --i)
		{
			if(proc != null)
				proc.job(mGrid.at(i, j0), i, j0);
			mMarked[i][j0] = true;
			++counter;
		}

		firstI = i + 1;
		
		for (i = i0; i < mColNum && needsProc(i, j0, rule); ++i)
		{
			if(proc != null)
				proc.job(mGrid.at(i, j0), i, j0);
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
						deque.add(new Point(i, j0 - 1));
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
						deque.add(new Point(i, j0 + 1));
					insideRowBelow = true;
				}

				else
					insideRowBelow = false;
			}
		}
		
		return counter;
	}

	public int run(int i0, int j0, boolean needReset, Rule<T> rule, Processor<T> proc)
	{
		Array<Point> deque = new Array<Point>();
		int counter = 0;

		if(needReset)
			reset();
		
		if (!needsProc(i0, j0, rule))
			return 0;
		
		deque.add(new Point(i0, j0));
		
		while (deque.size > 0)
		{
			Point p = deque.removeIndex(0);
			counter += procRow(deque, p.i, p.j, rule, proc);
		}
		
		return counter;
	}
	
	public int run(int i0, int j0, boolean needReset, Rule<T> rule)
	{
		return run(i0, j0, needReset, rule, null);
	}
	
	public boolean[][] getMarkMat()
	{
		return mMarked;
	}
	
	public boolean getMarkerAt(int i, int j)
	{
		if(i >= 0 && j >= 0 && i < mColNum && j < mRowNum)
			return mMarked[i][j];
		else
			return false;
	}*/
}

#endif
