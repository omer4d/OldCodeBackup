#ifndef WORKSPACE_HPP
#define WORKSPACE_HPP

#include <list>
#include <map>
#include <allegro.h>

#include "Vec2f.hpp"

#include "Document.hpp"
#include "Brush.hpp"

class Workspace {
	Document mDoc;
	std::list<Document> mPast;
	std::list<Document> mFuture;
	
	void commit()
	{
		mFuture.clear();
		mPast.push_front(mDoc);
	}
	
public:
	Workspace(BITMAP* src): mDoc(src, 25)
	{
	}
	
	void beginWrite()
	{
		commit();
		mDoc.beginWrite();
	}
	
	void endWrite()
	{
		mDoc.endWrite();
	}
	
	void brushStroke(Brush const& brush, Vec2f const& a, Vec2f const& b, float spacing)
	{
		if(spacing < 1)
			spacing = 1;
		
		int l = brush.getMaxSize() / 2;
		int r = brush.getSize() / 2;
		
		Vec2f d = (b - a).normalize() * spacing;
		float len = (a - b).length();
		Vec2f curr = a;
	
		for(float k = 0; k <= len; k += spacing)
		{
			mDoc.maskedBlit(brush.getBmp(), l - r, l - r, int(curr.x - r), int(curr.y - r),
							 2 * r + 1, 2 * r + 1);
			curr += d;
		}
	}
	
	int guessBgColor() const
	{
		std::map<int, int> colCount;
		
		for(int y = 0; y < mDoc.getHeight(); ++y)
			for(int x = 0; x < mDoc.getWidth(); ++x)
			{
				int col = mDoc.getPixel(x, y);
				auto iter = colCount.find(col);
				
				if(iter != colCount.end())
					++iter->second;
				else
					colCount[col] = 1;
			}
			
		int max = -1, maxCol = 0;
		
		for(auto iter: colCount)
		{
			if(iter.second > max)
			{
				max = iter.second;
				maxCol = iter.first;
			}
		}
		
		return maxCol;
	}
	
	void undo()
	{
		if(mPast.empty())
			return;
		
		mFuture.push_front(mDoc);
		mDoc = mPast.front();
		mPast.pop_front();
	}
	
	void redo()
	{
		if(mFuture.empty())
			return;
		
		mPast.push_front(mDoc);
		mDoc = mFuture.front();
		mFuture.pop_front();
	}
	
	void draw(BITMAP* dest, int x, int y) const
	{
		mDoc.draw(dest, x, y);
	}
};

#endif // WORKSPACE_HPP
